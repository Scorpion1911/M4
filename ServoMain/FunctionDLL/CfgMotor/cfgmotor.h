﻿#ifndef CFGMOTOR_H
#define CFGMOTOR_H

#include "cfgmotor_global.h"
#include "abstractfuncwidget.h"
namespace Ui {
class CfgMotor;
}
class QTreeWidget;
class QWidget;
class SqlTableModel;

class CFGMOTORSHARED_EXPORT CfgMotor:public AbstractFuncWidget
{
  Q_OBJECT
public:
  CfgMotor(QWidget *parent=0);
  ~CfgMotor(void);
  bool isWrite2Ram()Q_DECL_OVERRIDE {return false;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
  //use in qml
  Q_INVOKABLE bool getComConnectSatus();
  Q_INVOKABLE bool passChecked();
//  Q_INVOKABLE bool checkPrm();
public slots:
  void updateUiWhenNavigationTreeClicked()Q_DECL_OVERRIDE;
  void onWriteFuncTreetoServoFlash(void)Q_DECL_OVERRIDE;//与其它的不一样，因为保存到Flash时还要根据Imax额外保存gSevDrv.sev_obj.cur.adc.prm.kia,kib,kic信息

signals:
  void warningMessageChanged(QString &msg);
  void clearWarning(void);
//  void qmlEditUiStateChanged(int row,bool hasError);//检查参数时通知qml界面更改其状态
private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;
  void connectionSignalSlotHandler()Q_DECL_OVERRIDE;

  typedef struct{
    double gain;
    QString writeFlashName;
    QString writeFlashType;
    quint16 offsetAddr;
  }ImaxExtensionPrmGain;
  typedef struct{
    QString name;
    QString type;
    quint16 offsetAddr;
  }ImaxExtensionPrmImax;
  typedef struct{
    ImaxExtensionPrmImax imaxInfo;
    QList<ImaxExtensionPrmGain> gainInfoList;
  }ImaxExtensionPrm;

private:
  Ui::CfgMotor *ui;
//  QList<ImaxExtensinPrm>m_imaxExtPrm;
  SqlTableModel *m_companyTableModel;
  SqlTableModel *m_motorTableModel;
  SqlTableModel *m_prmTableModel;
  SqlTableModel *m_totalTableModel;
};

#endif // CFGMOTOR_H
