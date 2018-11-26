#pragma once
#include <Windows.h>
class CScreenSpy
{
public:
	CScreenSpy();
	CScreenSpy(ULONG BitmapCount);
	~CScreenSpy();
	LPBITMAPINFO CScreenSpy::OnInitBitmapInfo(ULONG BitmapCount,
		ULONG FullWidth, ULONG FullHeight);
	ULONG CScreenSpy::GetBitmapInfoSize();
	LPBITMAPINFO CScreenSpy::GetBitmapInfoData();
	LPVOID CScreenSpy::GetFirstScreenBufferData();
	ULONG CScreenSpy::GetFirstScreenBufferSize();
	LPVOID CScreenSpy::GetNextScreenBufferData(ULONG* BufferLength);
	VOID CScreenSpy::WriteScreenBufferData(LPBYTE	BufferData, ULONG BufferLength);
	VOID CScreenSpy::ScanScreen(HDC DestinationDCHandle, HDC SourceDCHandle, ULONG Width, ULONG Height);
	ULONG CScreenSpy::CompareBitmap(LPBYTE NextScreenBufferData, LPBYTE FirstScreenBufferData,
		LPBYTE BufferData, DWORD ScreenBufferLength);
private:
	ULONG  m_BitmapCount;
	int    m_FullWidth;
	int    m_FullHeight;
	HWND   m_DesktopHwnd;
	HDC    m_DesktopDCHandle;  
	HDC    m_DesktopMemoryDCHandle;
	LPBITMAPINFO     m_BitmapInfo;
	HBITMAP	m_BitmapHandle;
	PVOID   m_BitmapData;
	BYTE    m_Algorithm;
	
	HDC     m_DiffMemoryDCHandle;
	HBITMAP	m_DiffBitmapHandle;
	PVOID   m_DiffBitmapData;
	
	BYTE*  m_BufferData;
	ULONG  m_Offset;
};

