#pragma once
#include <windows.h>
#include <math.h>


class _declspec (dllexport) _CArray
{
public:
	_CArray();
	~_CArray();
	BOOL      _CArray::WriteArray(PUINT8 BufferData, ULONG_PTR BufferLength);   
	ULONG_PTR _CArray::ReAllocateArray(ULONG_PTR BufferLength);
	ULONG_PTR _CArray::GetArrayMaximumLength();
	ULONG_PTR _CArray::GetArrayLength();
	PUINT8    _CArray::GetArray(ULONG_PTR Pos = 0);
	VOID      _CArray::ClearArray();
	ULONG_PTR _CArray::DeAllocateArray(ULONG_PTR BufferLength);
	ULONG_PTR _CArray::ReadArray(PUINT8 BufferData, ULONG_PTR BufferLength);
	ULONG_PTR _CArray::RemoveComletedArray(ULONG_PTR BufferLength);
protected:
private:
	PUINT8	    m_BufferData;
	PUINT8	    m_BufferPtr;
	ULONG_PTR	m_MaximumLength;
	CRITICAL_SECTION  m_cs;

};

