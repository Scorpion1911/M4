#pragma once
#include "RingNetDriver.h"
typedef enum opt_type
{
	GTSD_COM_MODE_READ,			//读操作
	GTSD_COM_MODE_WRITE			//写操作
}OPT_TYPE;

typedef enum
{
	RN_PACKET_MAIL_RD_ASK = 0,
	RN_PACKET_MAIL_WR_ASK = 1,

	RN_PACKET_MAIL_RD_RESP = 2,
	RN_PACKET_MAIL_WR_RESP = 3,

	RN_PACKET_PDU_RD_ASK = 4,
	RN_PACKET_PDU_WR_ASK = 5,

	RN_PACKET_PDU_RD_RESP = 6,
	RN_PACKET_PDU_WR_RESP = 7
}ENUM_RnOpType;

typedef enum
{
	RN_OP_WAIT_DEFAULT = 0,
	RN_OP_WAIT_NORMAL = 1,
	RN_OP_WAIT_MEDIUM = 2,
	RN_OP_WAIT_LONG = 3,
	RN_OP_WAIT_INFINITE = 4
}ENUM_RnOpWaitLevel;
class CRingNetInterface :
	public CRingNetDriver
{
public:
	CRingNetInterface();
	~CRingNetInterface();




	const int COM_DSP_TRY_TIME = 1000;
	const int COM_FPGA_TRY_TIME = 200;
	/*******************************************************************************************
	功能：		从扫描到的设备中找到有响应的设备并打开该设备，找到一个有响应的设备后就退出
	输入：		无
	返回：		0成功，其他参看错误列表。
	*******************************************************************************************/
	int16 RnNetCom_Open(void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress);
	/*******************************************************************************************
	功能：关闭扫描到的有响应的设备
	输入：   无
	返回：   无
	*******************************************************************************************/
	int16 RnNetCom_Close();

	/*******************************************************************************************
	功能：FPGA数据通信交互函数
	输入：
	mode:			1:写  0：读
	dma_addr:		dma地址
	pData:			输入数据
	dma_num:		输入数据长度
	输出：pData:    输出数据
	dma_num:		输出数据长度
	返回：0成功，其他参看错误列表。
	*******************************************************************************************/
	int16 RnNetCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 word_num, int16 des_id, int16 needReq = RN_NEED_REQ);

	/*******************************************************************************************
	功能：dsp数据通信交互函数
	输入：
	mode:			1:写  0：读
	dma_addr:		dma地址
	pData:          输入数据
	word_num:		输入数据长度
	输出：pData:    输出数据
	word_num:		输出数据长度
	返回：0成功，其他参看错误列表。
	*******************************************************************************************/
	static const int32			NET_COM_EXECUTE_FAIL = 0;							// 命令执行失败
	static const int32			NET_COM_EXECUTE_SUCCESS = 1;							// 命令执行成功
	static const int32			NET_COM_PARAMETER_INVALID = 2;							// 无效参数
	static const int32			NET_COM_INSTRUCTION_INVALID = 3;							// 非法指令

	int16 RnNetCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 word_num, int16 des_id);
	int16 RnNetCom_DSP_ComHandler(int16 mode, int16 byte_addr, int16* pData, int16 word_num, Uint8 des_id, Uint8 des_ch);
	int16 RnNetComHandler(Uint8 mode, Uint16 byte_addr, int16* pData, Uint16 word_num, Uint8 des_id, Uint8 des_c, Uint8 needReq = TRUE, Uint8 addr_mod = RN_ADDR_INC, Uint8 wait_level = RN_OP_WAIT_DEFAULT, Uint8 dsitance = 0xf0);
private:
	int16 NC_Open(const int8* adapter, int32 port);
	int16 NC_Close(void);
};

//short UserRespDriver(StUserPacket* packet);