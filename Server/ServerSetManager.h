#pragma once
class CServerSetManager
{
public:
	CServerSetManager();
	~CServerSetManager();
	BOOL CServerSetManager::InitConfigFile();

	int GetInt(CString MainKey, CString SubKey);
	BOOL SetInt(CString MainKey, CString SubKey, int BufferData);

private:
	CString m_FileFullPathData;

};

