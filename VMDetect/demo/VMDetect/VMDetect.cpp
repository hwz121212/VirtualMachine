#include "stdafx.h"
#include "VMDetect.h"


void ShowError(char *pszText)
{
	char szErr[MAX_PATH] = {0};
	::wsprintf(szErr, "%s Error[%d]\n", pszText, ::GetLastError());
	::MessageBox(NULL, szErr, "ERROR", MB_OK);
}


// ʹ����Ȩָ�� IN ����Ƿ����������������
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
				// ��ebx����
				xor		ebx, ebx			
				// ָ�����ܺ�0x0a, ���ڻ�ȡVMWare�汾
				mov		ecx, 0x0A			
				// �˿ں�: 'VX' ---> 0x5658
				mov		edx, 'VX'		
				// �Ӷ˿�dx��ȡVMWare�汾��ebx
				in		eax, dx		
				// �ж�ebx���Ƿ�'VMXh',���������������
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


// ͨ������ MAC ��ַ����Ƿ����������������
BOOL VMDetect_MAC()
{
	BOOL bVM = FALSE;

	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	// ��ȡ������Ϣ
	DWORD dwStatus = ::GetAdaptersInfo(
		AdapterInfo,
		&dwBufLen);
	if (ERROR_SUCCESS != dwStatus)
	{
		ShowError("GetAdaptersInfo");
		return FALSE;
	}
	// �����������б���, ֵ��ȡ��һ������(Ĭ������)��Ϣ 
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
		// VMware ���� 
		bVM = TRUE;
	}

	return bVM;
}