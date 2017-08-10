//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton cmd layer Define		 						//
//	file				:	ServoDriverComDll.h											//
//	Description			:	use for cmd define											//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef			__GTSD_SERVO_MOTION_COM_DLL__
#define			__GTSD_SERVO_MOTION_COM_DLL__	



// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SERVODRIVERCOMDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SERVODRIVERCOMDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SERVODRIVERCOMDLL_EXPORTS
#define SERVODRIVERCOMDLL_API __declspec(dllexport)
#else
#define SERVODRIVERCOMDLL_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
#include "Basetype_def.h"
#include "BaseReturn_def.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
SERVODRIVERCOMDLL_API int16 GT_RN_Open(void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv);
SERVODRIVERCOMDLL_API int16 GT_RN_Close();

SERVODRIVERCOMDLL_API short GT_RN_PostPci(void* gPci, Uint8 station_id = 0xf0);
SERVODRIVERCOMDLL_API short GT_RN_SendPci(void* gPci, Uint8 station_id = 0xf0);

#endif