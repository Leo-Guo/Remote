#pragma once
#include <Windows.h>
#pragma warning(disable:4005)
#include <ntstatus.h>
#pragma warning(default:4005)
#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;
#pragma pack(1)
typedef struct _USN_NODE_
{
	ULONG64 ParentReferenceNumber;
	char FileNameData[MAX_PATH];
}USN_NODE, *PUSN_NODE;
#pragma pack(1)
typedef struct _FILE_NODE_
{
	ULONG64 FileReferenceNumber;
	char FileNameData[MAX_PATH];
}FILE_NODE, *PFILE_NODE;
class _CVolume
{
public:
	CCriticalSection m_CriticalSection;   //ÁÙ½çÇø
public:
	_CVolume(char ValumeValue);
	_CVolume(const _CVolume& C)
	{
		this->m_VolumeValue = C.m_VolumeValue;	
	}
	BOOL _CVolume::InitVolumeUSN();
	void _CVolume::ChangeFileMap(DWORD FileAttribute,
		ULONG64 FileReferenceNumber, DWORD Reason, ULONG64 ParentReferenceNumber, PWCHAR FileNameData, DWORD FileNameLength);
	void _CVolume::FindFile(ULONG64 FileReference, vector<FILE_NODE>& FindedFiles);
	ULONG64 FindParentReference(ULONG64 ReferenceNumber);
	~_CVolume();
public:
	char	m_VolumeValue;  //¾í±ê	
private:
	std::map<ULONG64, USN_NODE> m_USNFileMap;
};
