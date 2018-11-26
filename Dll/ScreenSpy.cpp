#include "ScreenSpy.h"




#define ALGORITHM_DIFF 1
CScreenSpy::CScreenSpy()
{

}


CScreenSpy::CScreenSpy(ULONG BitmapCount)
{
	
	m_Algorithm = ALGORITHM_DIFF; 
	switch (BitmapCount)
	{
	case 16:
	case 32:
	{
		m_BitmapCount = BitmapCount;
		break;
	}		
	default:
		m_BitmapCount = 16;
	}

	m_FullWidth  = GetSystemMetrics(SM_CXSCREEN);    //屏幕的分辨率
	m_FullHeight = GetSystemMetrics(SM_CYSCREEN);

	m_BitmapInfo = OnInitBitmapInfo(m_BitmapCount, m_FullWidth, m_FullHeight);  //构建位图信息 发送(第一波)



	//获得屏幕句柄
	m_DesktopHwnd = GetDesktopWindow();
	m_DesktopDCHandle = GetDC(m_DesktopHwnd);
	
	
	
	
	//第一帧
	m_DesktopMemoryDCHandle = CreateCompatibleDC(m_DesktopDCHandle);
	m_BitmapHandle = ::CreateDIBSection(m_DesktopDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_BitmapData, NULL, NULL);
	::SelectObject(m_DesktopMemoryDCHandle, m_BitmapHandle);


	m_DiffMemoryDCHandle = CreateCompatibleDC(m_DesktopDCHandle);
	m_DiffBitmapHandle = ::CreateDIBSection(m_DesktopDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_DiffBitmapData, NULL, NULL);
	::SelectObject(m_DiffMemoryDCHandle, m_DiffBitmapHandle);



	m_Offset = 0;
	m_BufferData = new BYTE[m_BitmapInfo->bmiHeader.biSizeImage*2];

}


CScreenSpy::~CScreenSpy()
{

	ReleaseDC(m_DesktopHwnd, m_DesktopDCHandle);   //GetDC
	if (m_DesktopMemoryDCHandle != NULL)
	{
		DeleteDC(m_DesktopMemoryDCHandle);                 //Create匹配内存DC

		::DeleteObject(m_BitmapHandle);
		if (m_BitmapData != NULL)
		{
			m_BitmapData = NULL;
		}

		m_DesktopMemoryDCHandle = NULL;

	}

	if (m_DiffMemoryDCHandle != NULL)
	{
		DeleteDC(m_DiffMemoryDCHandle);                //Create匹配内存DC

		::DeleteObject(m_DiffBitmapHandle);
		if (m_DiffBitmapData != NULL)
		{
			m_DiffBitmapData = NULL;
		}
	}

	if (m_BitmapInfo != NULL)   //发出信息头
	{
		delete[] m_BitmapInfo;
		m_BitmapInfo = NULL;
	}

	if (m_BufferData)    //发出的信息
	{
		delete[] m_BufferData;
		m_BufferData = NULL;
	}

	m_Offset = 0;
}
LPBITMAPINFO CScreenSpy::OnInitBitmapInfo(ULONG BitmapCount,
	ULONG FullWidth, ULONG FullHeight)//构建位图
{


	//[BITMAPINFOHEADER]
	ULONG BufferLength = sizeof(BITMAPINFOHEADER);
	BITMAPINFO* BitmapInfo = (BITMAPINFO *) new BYTE[BufferLength];

	BITMAPINFOHEADER* BitmapInfoHeader = &(BitmapInfo->bmiHeader);

	
	
	BitmapInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfoHeader->biWidth =  FullWidth; //1080
	BitmapInfoHeader->biHeight = FullHeight; //1920
	BitmapInfoHeader->biPlanes = 1;
	BitmapInfoHeader->biBitCount = BitmapCount; //16
	BitmapInfoHeader->biCompression = BI_RGB;
	BitmapInfoHeader->biXPelsPerMeter = 0;
	BitmapInfoHeader->biYPelsPerMeter = 0;
	BitmapInfoHeader->biClrUsed = 0;
	BitmapInfoHeader->biClrImportant = 0;
	BitmapInfoHeader->biSizeImage =
		((BitmapInfoHeader->biWidth * BitmapInfoHeader->biBitCount + 31) / 32) * 4 * BitmapInfoHeader->biHeight;

	return BitmapInfo;
}   //

ULONG CScreenSpy::GetBitmapInfoSize()
{
	return sizeof(BITMAPINFOHEADER);
}
LPBITMAPINFO CScreenSpy::GetBitmapInfoData()
{
	return m_BitmapInfo;
}
LPVOID CScreenSpy::GetFirstScreenBufferData()
{
	//用于从原设备中复制位图到目标设备
	::BitBlt(m_DesktopMemoryDCHandle, 0, 0,
		m_FullWidth, m_FullHeight, m_DesktopDCHandle, 0, 0, SRCCOPY);

	return m_BitmapData;  //内存
}
ULONG CScreenSpy::GetFirstScreenBufferSize()
{
	return m_BitmapInfo->bmiHeader.biSizeImage;
}
LPVOID CScreenSpy::GetNextScreenBufferData(ULONG* BufferLength)
{
	if (BufferLength == NULL || m_BufferData == NULL)
	{
		return NULL;
	}



	m_Offset = 0;


	// 写入使用了哪种算法
	WriteScreenBufferData((LPBYTE)&m_Algorithm, sizeof(m_Algorithm));

	// 写入光标位置
	POINT	CursorPos;
	GetCursorPos(&CursorPos);
	WriteScreenBufferData((LPBYTE)&CursorPos, sizeof(POINT));

	// 写入当前光标类型
	BYTE	CursorIndex = -1; //随便写一个
	WriteScreenBufferData(&CursorIndex, sizeof(BYTE));



	//m_BufferData [ALGORITHM_DIFF 4X 4Y BYTE                                                      ]  m_rectBufferOffset = 10



	// 差异比较算法
	if (m_Algorithm == ALGORITHM_DIFF)
	{
		// 分段扫描全屏幕  将新的位图放入到m_hDiffMemDC中
		ScanScreen(m_DiffMemoryDCHandle, m_DesktopDCHandle, m_BitmapInfo->bmiHeader.biWidth,
			m_BitmapInfo->bmiHeader.biHeight);

		//两个Bit进行比较如果不一样修改m_lpvFullBits中的返回
		*BufferLength = m_Offset +
			CompareBitmap((LPBYTE)m_DiffBitmapData, (LPBYTE)m_BitmapData,
				m_BufferData + m_Offset, m_BitmapInfo->bmiHeader.biSizeImage);
		return m_BufferData;
	}
	
	return NULL;
}

ULONG CScreenSpy::CompareBitmap(LPBYTE NextScreenBufferData, LPBYTE FirstScreenBufferData,
	LPBYTE BufferData, DWORD ScreenBufferLength)
{
	// Windows规定一个扫描行所占的字节数必须是4的倍数, 所以用DWORD比较
	LPDWORD	v1, v2;
	v1 = (LPDWORD)FirstScreenBufferData;
	v2 = (LPDWORD)NextScreenBufferData;

	//m_NextScreenBufferData [0004 0003 000A 000A 000A 0007 0000 ]  m_rectBufferOffset = 10

	// 偏移的偏移，不同长度的偏移
	ULONG Offset = 0, v11 = 0, v22 = 0;
	ULONG Count = 0; // 数据计数器
					   // p1++实际上是递增了一个DWORD
	for (int i = 0; i < ScreenBufferLength; i += 4, v1++,v2++)   //HellAAArldlAD1C   HellAAArld1AD1A     BufferData[Pos][3][lAD][Pos][][]
	{
		if (*v1 == *v2)
			continue;
		// 一个新数据块开始
		// 写入偏移地址

		*(LPDWORD)(BufferData + Offset) = i; //位置
		// 记录数据大小的存放位置
		v11 = Offset + sizeof(int);  //4
		v22 = v11 + sizeof(int);     //8
		Count = 0; // 数据计数器归零

		// 更新前一帧数据
		*v1 = *v2;
		*(LPDWORD)(BufferData + v22 + Count) = *v2;

		Count += 4;
		i += 4, v1++, v2++;
		for (int j = i; j < ScreenBufferLength; j += 4, i += 4, v1++, v2++)
		{
			if (*v1 == *v2)
				break;

			// 更新Dest中的数据
			*v1 = *v2;
			*(LPDWORD)(BufferData + v22 + Count) = *v2;
			Count += 4;
		}
		// 写入数据长度
		*(LPDWORD)(BufferData + v11) = Count;
		Offset = v22 + Count;
	}

	// nOffsetOffset 就是写入的总大小
	return Offset;
}




VOID CScreenSpy::WriteScreenBufferData(LPBYTE	BufferData, ULONG BufferLength)
{
	memcpy(m_BufferData + m_Offset, BufferData, BufferLength);
	m_Offset += BufferLength;
}

VOID CScreenSpy::ScanScreen(HDC DestinationDCHandle, HDC SourceDCHandle, ULONG Width, ULONG Height)
{
	ULONG	JumpLine = 50;
	ULONG	JumpSleep = JumpLine / 10;

	for (int i = 0, ToJump = 0; i < Height; i += ToJump)
	{
		ULONG  v1 = Height - i;
		if (v1 > JumpLine)
			ToJump = JumpLine;
		else
			ToJump = v1;
		BitBlt(DestinationDCHandle, 0, i, Width, ToJump, SourceDCHandle, 0, i, SRCCOPY);
		Sleep(JumpSleep);
	}



}