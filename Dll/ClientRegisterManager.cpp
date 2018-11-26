#include "ClientRegisterManager.h"



CClientRegisterManager::CClientRegisterManager(CIOCPClient* ClientObject):CManager(ClientObject)
{
	printf("注册表构造函数\r\n");
/*	m_RegisterModuleBase = LoadLibrary("Service.dll");
	if (m_RegisterModuleBase == NULL)
	{

	}
	m_SeEnumServiceList = (LPFN_SEENUMSERVICELIST)GetProcAddress(m_ServiceModuleBase, "SeEnumServiceList");
	m_SeCofigService = (LPFN_SECONFIGSERVICE)GetProcAddress(m_ServiceModuleBase, "SeCofigService");
	if (m_SeEnumServiceList == NULL || m_SeCofigService == NULL)
	{
		return;
	}
	SendClientRegisterList();*/
}


CClientRegisterManager::~CClientRegisterManager()
{
}
