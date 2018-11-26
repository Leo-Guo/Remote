#pragma once
#include "Manager.h"
class CClientRegisterManager :
	public CManager
{
public:
	CClientRegisterManager(CIOCPClient* ClientObject);
	~CClientRegisterManager();
};

