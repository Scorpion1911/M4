﻿#include "cfgmotor.h"
#include "ui_cfgmotor.h"
#include "MainGTSD/mainwindow.h"
#include <MainGTSD/MotorSqlModel/motorsqlmodel.h>
#include <MainGTSD/NavigationConfig/DataBase/SqltableModel/sqltablemodel.h>

#include <QQuickWidget>
#include <QQmlContext>


CfgMotor::CfgMotor(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgMotor())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgMotor::~CfgMotor()
{
  qDebug()<<"CfgMotor release ->";
}

bool CfgMotor::getComConnectSatus()
{
  return mp_mainWindow->getComOpenState();
}
bool CfgMotor::checkPrm()
{
  bool ok=true;
  if(m_uiTree!=NULL)
  {
    QTreeWidgetItem *item;
    QString name;
    double value;
    double kgain;
    double max;
    double min;
    for(int i=0;i<m_uiTree->topLevelItemCount();i++)
    {
      item=m_uiTree->topLevelItem(i);
      name=item->text(COL_FUNC_NAME);
      kgain=GlobalFunction::cmdKgain(name,mp_mainWindow->getFunctionCmdTree());
      value=kgain*item->text(COL_FUNC_VALUE).toDouble();
      qDebug()<<"value="<<value;
      max=item->text(COL_FUNC_UPLIMIT).toDouble();
      min=item->text(COL_FUNC_DOWNLIMIT).toDouble();
      //发送消息到qml，还原颜色

      if(!(value>=min&&value<=max))
      {
        ok=false;
        //发送错误消息到qml界面，变红色
        qDebug()<<"error:"<<name;
        break;
      }
      qDebug()<<name<<"-----OK";
    }
  }
  return ok;
}

void CfgMotor::updateUiWhenNavigationTreeClicked()
{
  onActionReadFuncValueFromFlash();
}
void CfgMotor::onWriteFuncTreetoServoFlash()
{
  bool needChecked=prmNeedChecked();
  qDebug()<<"Need Check="<<needChecked;
  if(needChecked)
  {
    if(false==checkPrm())
      return;
  }
  ImaxExtensionPrm imaxPrm;
  ImaxExtensionPrmGain gainInfo;

  QTreeWidgetItem *imaxTreeItem;
  QTreeWidgetItem *imaxInfoItem;
  QTreeWidgetItem *gainInfoItem;
  AbstractFuncWidget::onWriteFuncTreetoServoFlash();
//  mp_mainWindow->clearWarning();
  emit clearWarning();
  imaxTreeItem=mp_mainWindow->getFunctionExtensionTree()\
          ->topLevelItem(ROW_FUNC_EXT_INDEX_MODULEPRM)\
          ->child(ROW_MODULEPRM_INDEX_MOTOR)\
          ->child(m_axisNumber);
  qDebug()<<"imaxTree: "<<imaxTreeItem->text(COL_FUNC_EXTENSION_NAME);
  imaxInfoItem=imaxTreeItem->child(ROW_MOTOR_AXIS_INDEX_IMAXINFO);
  gainInfoItem=imaxTreeItem->child(ROW_MOTOR_AXIS_INDEX_GAININFO);
  imaxPrm.imaxInfo.name=imaxInfoItem->child(ROW_IMAXINFO_INDEX_NAME)->text(COL_FUNC_EXTENSION_PARAMETER);
  imaxPrm.imaxInfo.type=imaxInfoItem->child(ROW_IMAXINFO_INDEX_TYPE)->text(COL_FUNC_EXTENSION_PARAMETER);
  imaxPrm.imaxInfo.offsetAddr=imaxInfoItem->child(ROW_IMAXINFO_INDEX_OFFSETADDR)->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();

  for(int i=0;i<gainInfoItem->childCount();i++)
  {
    QTreeWidgetItem *item=gainInfoItem->child(i);
    qDebug()<<"child name: "<<item->text(COL_FUNC_EXTENSION_NAME);
    gainInfo.gain=item->text(COL_FUNC_EXTENSION_PARAMETER).toDouble();
    gainInfo.writeFlashName=item->child(0)->child(ROW_GAINWRFLASH_INDEX_NAME)->text(COL_FUNC_EXTENSION_PARAMETER);
    gainInfo.writeFlashType=item->child(0)->child(ROW_GAINWRFLASH_INDEX_TYPE)->text(COL_FUNC_EXTENSION_PARAMETER);
    gainInfo.offsetAddr=item->child(0)->child(ROW_GAINWRFLASH_INDEX_OFFSETADDR)->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
    imaxPrm.gainInfoList.append(gainInfo);
  }

  qDebug()<<"ImaxInfo: "<<" name:"<<imaxPrm.imaxInfo.name<<" type:"<<imaxPrm.imaxInfo.type<<" offset:"<<imaxPrm.imaxInfo.offsetAddr;
  foreach (gainInfo, imaxPrm.gainInfoList) {
    qDebug()<<"list in imaxExtPrmList: "<<gainInfo.writeFlashName<<" gain:"<<gainInfo.gain<<" type:"<<gainInfo.writeFlashType<<" offsetAddr:"<<gainInfo.offsetAddr;
  }
  //读Imax的原始值
  Uint16 imaxValue;
  int16 ret=0;
  UserConfig *config=mp_mainWindow->getUserConfig();
  ret=GTSD_CMD_Fram_Read16BitByAdr(m_axisNumber, (int16) imaxPrm.imaxInfo.offsetAddr, (int16*) &imaxValue, config->com.id, config->com.rnStation);
  if(ret!=0)
     ret=GTSD_CMD_Fram_Read16BitByAdr(m_axisNumber, (int16) imaxPrm.imaxInfo.offsetAddr, (int16*) &imaxValue, config->com.id, config->com.rnStation);
  qDebug()<<"imax value:"<<imaxValue;
  foreach (gainInfo, imaxPrm.gainInfoList)
  {
    double k=gainInfo.gain/imaxValue;
    if(k>32767)
    {
      k=32767;
//      mp_mainWindow->setWarningMessage(tr("Imax is too small!"));
      emit showMessage(tr("Imax is too small!"));
    }
    int16 value=(int16)k;
    ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisNumber,gainInfo.offsetAddr,value,config->com.id, config->com.rnStation);
    if(ret!=0)
      ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisNumber,gainInfo.offsetAddr,value,config->com.id, config->com.rnStation);
  }
}

//!-------------------------private function--------------------------
QTreeWidget* CfgMotor::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgMotor::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void CfgMotor::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}
void CfgMotor::createUiByQml()
{
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/ui/"+objectName()+".qml";
  qDebug()<<tr("******************qml path:%1****************\r\n*****icon path %2********").arg(m_qmlFilePath).arg(m_qmlIconFilePath);

  qmlRegisterType<SqlTableModel>("QmlGlobalClass", 1, 0, "SqlTableModel");

  MotorSqlModel* motorSqlModel=mp_mainWindow->getMotorSqlModel();
  QSqlDatabase *db=motorSqlModel->db();

  m_companyTableModel=new SqlTableModel(this,*db);
  m_motorTableModel=new SqlTableModel(this,*db);
  m_prmTableModel=new SqlTableModel(this,*db);
  m_totalTableModel=new SqlTableModel(this,*db);

  m_companyTableModel->setTable("Company");
  m_companyTableModel->select();

  m_motorTableModel->setTable("Motor");
  m_motorTableModel->setFilter("CompanyId=1");
  m_motorTableModel->select();

  m_prmTableModel->setTable("Motor");
  m_prmTableModel->select();

  m_totalTableModel->setTable("Motor");
  m_totalTableModel->select();

//  m_company=motorSqlModel->companyTableModel();
//  m_motor=motorSqlModel->motorTableModel();
//  m_prm=motorSqlModel->prmTableModel();
//  m_total=motorSqlModel->totalTableModel();
//  qDebug()<<tr("*************step1********");
//  qDebug()<<"motor:  "<<(quint64)&m_motor;

  QQuickWidget *qwidget=new QQuickWidget(this);
  qwidget->setMinimumSize(600,560);
  qwidget->rootContext()->setContextProperty("iconPath",m_qmlIconFilePath);
  qwidget->rootContext()->setContextProperty("driveMotor",this);
  qwidget->rootContext()->setContextProperty("axisIndex",m_axisNumber);
  qwidget->rootContext()->setContextProperty("treeSource",ui->treeWidget);
  qwidget->rootContext()->setContextProperty("companyModel",m_companyTableModel);
  qwidget->rootContext()->setContextProperty("motorModel",m_motorTableModel);
  qwidget->rootContext()->setContextProperty("prmModel",m_prmTableModel);
  qwidget->rootContext()->setContextProperty("totalModel",m_totalTableModel);
  qDebug()<<tr("*************step2********");

  qwidget->resize(600,560);
  qwidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
  qDebug()<<tr("*************step3********");
  qwidget->setSource(QUrl::fromLocalFile(m_qmlFilePath));
  qDebug()<<tr("*************step4********");
//  qwidget->show();
  ui->qmlHboxLayout->addWidget(qwidget);
  qDebug()<<tr("*************step5********");
}
void CfgMotor::connectionSignalSlotHandler()
{
  connect(this,SIGNAL(warningMessageChanged(QString&)),mp_mainWindow,SLOT(onWarningMessageChanged(QString&)));
  connect(this,SIGNAL(clearWarning()),mp_mainWindow,SLOT(onClearWarning()));
}
