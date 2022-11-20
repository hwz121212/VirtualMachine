# 两种方法实现虚拟机检测

# 背景

虚拟机因为它的便捷易用性，使得它被越来越多人喜爱。相信大家也都会在自己的计算机上面安装有虚拟机，平时也会使用虚拟机来测试程序或者做一些文件分析工作。所谓的虚拟机（Virtual Machine）是指通过软件模拟的具有完整硬件系统功能的、运行在一个完全隔离环境中的完整计算机系统。通过虚拟机软件（比如VMware、Virtual PC、VirtualBox），你可以在一台物理计算机上模拟出一台或多台虚拟的计算机，这些虚拟机完全就像真正的计算机那样进行工作。

但是，无论是模拟得多好的虚拟机，终归只是虚拟机，肯定会留下些属于自己特有的痕迹，使得程序可以判断出自己是运行在虚拟机当中。本文就是要是现实这样的一个程序，来判断自己是运行在虚拟机中还是真机当中。现在，我就把实现过程整理成文档，分享给大家。

# 实现原理

本文介绍两种虚拟机检测方法，分别是使用执行特权指令来检测虚拟机，另一种是通过网卡MAC地址检测虚拟机。

通过执行特权指令 IN 来检测虚拟机的原理是：

VMware 为真主机与虚拟机之间提供了相互沟通的通讯机制，它使用 IN 指令来读取特定端口的数据以进行两机通讯。但由于 IN 指令属于特权指令，在处于保护模式下的真机上执行此指令时，除非权限允许，否则将会触发类型为 EXCEPTION_PRIV_INSTRUCTION 的异常，而在虚拟机中并不会发生异常。

在指定功能号 0x0A（获取VMware版本）的情况下，当 EBX 中返回其版本号 “VMXH”时，则说明处于虚拟机中；而当功能号为 0x14 时，可用于获取 VMware 内存大小，当返回值 EAX 大于 0 时，则说明处于虚拟机中。

通过网卡 MAC 地址检测原理是：

VMware默认的网卡MAC地址前缀为 00-05-69、00-0C-29 或者 00-50-56，这前 3 字节是由 VMware 分配的唯一标识符 OUI，以供它的虚拟化适配器使用。所以，只要我们获取 MAC 地址，然后判断它前 3 字节的数据，若符合 WMware 的默认前缀，则则说明处于虚拟机中。

其中，获取计算机上的 MAC 地址，可以参考我写的《编程获取计算机MAC地址等网卡信息并对网卡类型进行区分》这篇文章。

# 编码实现

## 使用特权指令 IN 检测

```c++
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
```

## 使用默认网卡 MAC 地址检测

```c++
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
```

# 测试

当我们在真机上运行程序的时候，两种检测方法均能正确检测出运行在真机上：

![](http://www.writebug.com/myres/static/uploads/2021/10/19/94d9e798c64a5b8f384a392216f90df8.writebug)

当我们在 VMware 虚拟机机上运行程序的时候，两种检测方法均能正确检测出运行在虚拟机里：

![](http://www.writebug.com/myres/static/uploads/2021/10/19/8e365b47dc1b2aecd288910e2c701a91.writebug)

# 总结

虚拟机检测技术有很多，当然不止是这两种。本文只是抛砖引玉，向大家讲解目前比较流行的两种检测方式。当然，感兴趣的同学，可以继续往下钻研，积累更多的检测虚拟机的方法。

# 参考

参考自《[Windows黑客编程技术详解](https://www.write-bug.com/article/1811.html "Windows黑客编程技术详解")》一书