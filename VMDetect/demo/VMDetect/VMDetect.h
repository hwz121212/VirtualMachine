#ifndef _VM_DETECT_H_
#define _VM_DETECT_H_


#include <Windows.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


// ʹ����Ȩָ�� IN ����Ƿ����������������
BOOL VMDetect_IN();

// ͨ������ MAC ��ַ����Ƿ����������������
BOOL VMDetect_MAC();


#endif