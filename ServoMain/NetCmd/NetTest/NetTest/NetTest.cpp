// NetTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ServoDriverComDll.h"
#include <iostream>
using namespace std;
void updateProgress(void *arg, int16 *value);
#define TEST_UBOOT 1

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0x00ff;
	int aa = 100;
	int comType = 1;
	ret = GTSD_CMD_Open(updateProgress, (void *)&aa, comType);
	cout << "ret=" << ret;
	//GTSD_CMD_Close(comType);
#if TEST_UBOOT
	std::wstring ldrPath = L"D:/Smart/ServoMaster/git-project/servo-4/release/Resource/Uboot/ServoUboot.ldr";
	std::string key = "a5e4b8a4d71d04d2f89d8318fec19283";
	GTSD_CMD_ProcessorUartBootHandler(0, ldrPath, 115200, 1, key, updateProgress, NULL, 1, 1);
#endif

	while (true)
	{

	}
	return 0;
}
void updateProgress(void *arg, int16 *value)
{
	int16 v = *value;
	cout << "value=" << v << endl;
}

