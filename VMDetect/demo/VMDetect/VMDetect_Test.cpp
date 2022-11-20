// VMDetect_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VMDetect.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// 使用特权指令 IN 检测
	if (VMDetect_IN())
	{
		printf("Is In VM. [VMDetect_IN]\n");
	}
	else
	{
		printf("Is NOT In VM. [VMDetect_IN]\n");
	}
	// 使用网卡 MAC 地址检测
	if (VMDetect_MAC())
	{
		printf("Is In VM. [VMDetect_MAC]\n");
	}
	else
	{
		printf("Is NOT In VM. [VMDetect_MAC]\n");
	}

	system("pause");
	return 0;
}

