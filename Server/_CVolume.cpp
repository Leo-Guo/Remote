#include "stdafx.h"
#include "_CVolume.h"


_CVolume::_CVolume(char VolumeValue)
{
	m_VolumeValue = VolumeValue;
}
_CVolume::~_CVolume()
{
}


BOOL _CVolume::InitVolumeUSN()
{
	CHAR   VolumeValue[] = "A:\\";
	HANDLE DiskVolumeHandle = NULL;
	CHAR   DiskVolumeName[] = "\\\\.\\A:";
	DWORD  ReturnLength = 0;
	BOOL   IsOk = FALSE;
	CHAR   USNDataBuffer[0x4000] = { 0 };
	INT    ErrorCode = 0;
	USN_NODE USNNode = { 0 };

	VolumeValue[0] = m_VolumeValue;

	m_CriticalSection.Lock();  
	
	// ÿ���̵��̷�����һ���� 0x5000000000005
	m_USNFileMap[0x5000000000005].ParentReferenceNumber = 0;
	memcpy(m_USNFileMap[0x5000000000005].FileNameData,VolumeValue,strlen(VolumeValue)+1);


	//C:\  D:\   Ring

	// ���¹����ַ��� �򿪴��̾��
	DiskVolumeName[4] = m_VolumeValue;		// \\.\ + �̷���   example: \\.\C:
										// ���̷� ����Ҫ��UACȨ��
	DiskVolumeHandle = CreateFileA(DiskVolumeName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (DiskVolumeHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// ����DeviceIoControl ���UNS
	PVOID USNJournalData = VirtualAlloc(NULL, sizeof(USN_JOURNAL_DATA), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	ZeroMemory(USNJournalData, sizeof(USN_JOURNAL_DATA));
	IsOk = DeviceIoControl(DiskVolumeHandle, FSCTL_QUERY_USN_JOURNAL, NULL, 0, USNJournalData, 
		sizeof(USN_JOURNAL_DATA), &ReturnLength, NULL);
	if (IsOk)
	{

		MFT_ENUM_DATA_V0 MftEnumData = { 0 };
		MftEnumData.StartFileReferenceNumber = 0;
		MftEnumData.LowUsn = 0;
		MftEnumData.HighUsn = ((PUSN_JOURNAL_DATA)USNJournalData)->NextUsn;
		PUSN_RECORD USNRecordData = NULL;
		while (DeviceIoControl(DiskVolumeHandle, FSCTL_ENUM_USN_DATA, &MftEnumData, 
			sizeof(MFT_ENUM_DATA_V0), USNDataBuffer, 0x4000, &ReturnLength, NULL) != FALSE)
		{
		
			DWORD USNRecordLength = ReturnLength - sizeof(USN);

			USNRecordData = (PUSN_RECORD)(((PCHAR)USNDataBuffer) + sizeof(USN));
			while (USNRecordLength > 0)
			{
				const int FileNameLength = USNRecordData->FileNameLength;
				char FileNameData[MAX_PATH] = { 0 };
				WideCharToMultiByte(CP_OEMCP, NULL, USNRecordData->FileName, FileNameLength / 2, 
					FileNameData, FileNameLength, NULL, FALSE);

				// ����MAP�ڵ㣬����MAP
				USNNode.ParentReferenceNumber = USNRecordData->ParentFileReferenceNumber;
			
				memcpy(USNNode.FileNameData, FileNameData, MAX_PATH);
				
		
				m_USNFileMap[USNRecordData->FileReferenceNumber] = USNNode;
				// ��ȡ��һ����¼  
				DWORD RecordLength = USNRecordData->RecordLength;
				USNRecordLength -= RecordLength;
				USNRecordData = (PUSN_RECORD)(((PCHAR)USNRecordData) + RecordLength);
			}
			MftEnumData.StartFileReferenceNumber = *(USN *)&USNDataBuffer;
		}
	}
	else
	{
		ErrorCode = GetLastError();
	
		switch (ErrorCode)
		{
		case ERROR_JOURNAL_NOT_ACTIVE:
		{
		
			break;
		}
		case ERROR_INVALID_PARAMETER:
		{
			
			break;
		}
		case ERROR_JOURNAL_DELETE_IN_PROGRESS:
		{
			
			break;
		}
		default:
			break;
		}
	}

	m_CriticalSection.Unlock();
	VirtualFree(USNJournalData, sizeof(USN_JOURNAL_DATA), MEM_RELEASE);
	CloseHandle(DiskVolumeHandle);

	DiskVolumeHandle = NULL;


	return IsOk;
}


void _CVolume::ChangeFileMap(DWORD FileAttribute, 
	ULONG64 FileReferenceNumber, DWORD Reason, ULONG64 ParentReferenceNumber, PWCHAR FileNameData, DWORD FileNameLength)
{
	CHAR v1[MAX_PATH] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, FileNameData, FileNameLength,v1 , MAX_PATH, 0, 0);

	USN_NODE USNNode = { 0 };

	USNNode.ParentReferenceNumber = ParentReferenceNumber;
	memcpy(USNNode.FileNameData, v1, MAX_PATH);

	BOOL IsDirctory = FileAttribute & FILE_ATTRIBUTE_DIRECTORY;

	if ((USN_REASON_FILE_CREATE & Reason) && (USN_REASON_CLOSE & Reason))
	{
		// �ļ�����,����Map
		m_USNFileMap[FileReferenceNumber] = USNNode;
	}
	//�������ļ���Ŀ¼
	else if ((USN_REASON_RENAME_NEW_NAME & Reason) && (Reason & USN_REASON_CLOSE))
	{
		//��������������
		m_USNFileMap[FileReferenceNumber] = USNNode;
	}
	//�ļ���Ŀ¼��������
	else if (USN_REASON_RENAME_OLD_NAME & Reason)
	{
		if (USN_REASON_FILE_CREATE & Reason)
		{
			// �����ظ����
		}
		//����ԭ���ļ�������
		else
		{
			if (IsDirctory)
				m_USNFileMap[FileReferenceNumber] = USNNode;
			else
				m_USNFileMap.erase(FileReferenceNumber);
		}
	}
	//�ļ���Ŀ¼��ɾ����
	else if ((Reason & USN_REASON_FILE_DELETE) && (USN_REASON_CLOSE & Reason))
	{
		if (USN_REASON_FILE_CREATE & Reason)
		{
			// �����ظ����
		}
		else
		{
			//������Ŀ¼ ����Ŀ¼�仯
			m_USNFileMap.erase(FileReferenceNumber);
		}
	}
	else
	{
		
	}
}


void _CVolume::FindFile(ULONG64 FileReferenceNumber, vector<FILE_NODE>& FindedFiles)
{
	
	FILE_NODE v1 = { 0 };	

	
	m_CriticalSection.Lock();
	for (map<ULONG64, USN_NODE>::iterator i = m_USNFileMap.begin(); 
		i != m_USNFileMap.end(); i++)
	{
		if (i->second.ParentReferenceNumber == FileReferenceNumber)
		{
				
			memcpy(v1.FileNameData, i->second.FileNameData, 
				strlen(i->second.FileNameData)+1);
			v1.FileReferenceNumber = i->first;
			FindedFiles.push_back(v1);
		}
	}
	m_CriticalSection.Unlock();
}
ULONG64 _CVolume::FindParentReference(ULONG64 ReferenceNumber)
{
	m_CriticalSection.Lock();
	std::map<ULONG64, USN_NODE>::iterator i = m_USNFileMap.find(ReferenceNumber);
	if (i != m_USNFileMap.end())
	{
		return i->second.ParentReferenceNumber;
	}
	m_CriticalSection.Unlock();
}