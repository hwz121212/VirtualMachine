// VMDetect_Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "VMDetect.h"


int _tmain(int argc, _TCHAR* argv[])
{
	// ʹ����Ȩָ�� IN ���
	if (VMDetect_IN())
	{
		printf("Is In VM. [VMDetect_IN]\n");
	}
	else
	{
		printf("Is NOT In VM. [VMDetect_IN]\n");
	}
	// ʹ������ MAC ��ַ���
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

