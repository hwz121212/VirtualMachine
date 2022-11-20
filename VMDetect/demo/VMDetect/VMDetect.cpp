#include "stdafx.h"
#include "VMDetect.h"


void ShowError(char *pszText)
{
	char szErr[MAX_PATH] = {0};
	::wsprintf(szErr, "%s Error[%d]\n", pszText, ::GetLastError());
	::MessageBox(NULL, szErr, "ERROR", MB_OK);
}


// 使用特权指令 IN 检测是否运行在虚拟机当中
BOOL VMDetect_IN()
{
	BOOL bVM = FALSE;

	__try
	{
		__asm
		{
			    push	edx
				push	ecx
				push	ebx

				// 'VMXh' ---> 0x564d5868
				mov		eax, 'VMXh'    
				// 将ebx清零
				xor		ebx, ebx			
				// 指定功能号0x0a, 用于获取VMWare版本
				mov		ecx, 0x0A			
				// 端口号: 'VX' ---> 0x5658
				mov		edx, 'VX'		
				// 从端口dx读取VMWare版本到ebx
				in		eax, dx		
				// 判断ebx中是否'VMXh',若是则在虚拟机中
				cmp		ebx, 'VMXh' 	    
				je      _vm
				jmp     _exit
			_vm:
				mov     eax, TRUE
				mov     bVM, eax
          _exit:
				pop		ebx
				pop		ecx
				pop		edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("EXCEPTION_EXECUTE_HANDLER because of IN.\n");
		bVM = FALSE;
	}

	return bVM;
}


// 通过网卡 MAC 地址检测是否运行在虚拟机当中
BOOL VMDetect_MAC()
{
	BOOL bVM = FALSE;

	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	// 获取网卡信息
	DWORD dwStatus = ::GetAdaptersInfo(
		AdapterInfo,
		&dwBufLen);
	if (ERROR_SUCCESS != dwStatus)
	{
		ShowError("GetAdaptersInfo");
		return FALSE;
	}
	// 不对网卡进行遍历, 值获取第一个网卡(默认网卡)信息 
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	if (
		// 00-05-69	
		(0x00 == pAdapterInfo->Address[0] &&
		0x05 == pAdapterInfo->Address[1] &&
		0x69 == pAdapterInfo->Address[2]) ||
		// 00-0C-29
		(0x00 == pAdapterInfo->Address[0] &&
		0x0c == pAdapterInfo->Address[1] &&
		0x29 == pAdapterInfo->Address[2]) ||
		// 00-50-56
		(0x00 == pAdapterInfo->Address[0] &&
		0x50 == pAdapterInfo->Address[1] &&
		0x56 == pAdapterInfo->Address[2])
		)
	{
		// VMware 网卡 
		bVM = TRUE;
	}

	return bVM;
}