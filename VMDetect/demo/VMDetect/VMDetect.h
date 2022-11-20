#ifndef _VM_DETECT_H_
#define _VM_DETECT_H_


#include <Windows.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


// 使用特权指令 IN 检测是否运行在虚拟机当中
BOOL VMDetect_IN();

// 通过网卡 MAC 地址检测是否运行在虚拟机当中
BOOL VMDetect_MAC();


#endif