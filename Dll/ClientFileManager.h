#pragma once
#include "Manager.h"
#include "Common.h"


#pragma pack(1)
typedef
struct _HARD_DISK_INFORMATION_
{
	char  HardDiskName[2];
	char  DisplayName[MAX_PATH];
	DWORD HardDiskIcon;
	BOOL  IsDirectory;
	char  FileSystem[MAX_PATH + 1];
	char  HardDiskType[80];
	float HardDiskAmountMB;
	float HardDiskFreeSpaceMB;
}HARD_DISK_INFORMATION;
typedef SIZE_T (*LPFN_GETLOCALHARDDISKINFO)(vector<_HARD_DISK_INFORMATION_>& HardDiskInfo);

#pragma pack(1)
typedef struct _FILE_OBJECT_INFORMATION_
{
	BOOL IsDirectory;
	char FileName[MAX_PATH];
	DWORD FileSizeHigh;
	DWORD FileSizeLow;
	FILETIME LastWriteTime;
}FILE_OBJECT_INFORMATION;
typedef SIZE_T(*LPFN_GETLOCALFILELIST)(char* DirectoryPathData,
	vector<FILE_OBJECT_INFORMATION>& FileObjectInfo);



class CClientFileManager :
	public CManager
{
public:
	CClientFileManager(CIOCPClient* ClientObject);
	~CClientFileManager();
	BOOL CClientFileManager::SendClientFileInfo();
	int  CClientFileManager::SendClientFileList(char* DirectoryPathData);
	virtual void CClientFileManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
private:
	HMODULE m_FileModuleBase = NULL;
	LPFN_GETLOCALHARDDISKINFO m_GetLocalHardDiskInfo = NULL;
	LPFN_GETLOCALFILELIST     m_GetLocalFileList = NULL;
};

