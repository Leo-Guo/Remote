#pragma once

#include "IOCPClient.h"
#include <Vfw.h>
#include <winternl.h>
#include <ntstatus.h>
#pragma comment(lib,"Vfw32.lib")

#pragma pack(1)
typedef struct  _LOGIN_INFO
{
	BYTE			IsToken;		    //��Ϣͷ��
	OSVERSIONINFOEX	OsVersionInfoEx;	// �汾��Ϣ
	char ProcessorNameStringData[MAX_PATH];	// CPU��Ƶ
	IN_ADDR			ClientAddressData;		// �洢32λ��IPv4�ĵ�ַ���ݽṹ
	char			HostNameData[MAX_PATH];	// ������
	BOOL			IsWebCameraExist;		// �Ƿ�������ͷ
	DWORD			WebSpeed;		// ����
}LOGIN_INFO, *PLOGIN_INFOR;

int SendLoginInformtion(CIOCPClient* ClientObject,DWORD WebSpeed);
NTSTATUS ProcessorNameString(char* ProcessorNameStringData, ULONG* ProcessorNameStringLength);
BOOL IsWebCamera();

