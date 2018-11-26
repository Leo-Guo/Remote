// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;


//Test  
//加载Dll库 搜索导出函数 调用

struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;
	char  ServerIP[20];
	int   ServerPort;
}__ServerConnectInfo = { 0x99999999,"192.168.1.2",2356};


typedef \
void(*pfnRun911)(char* ServerIP,USHORT ServerPort);

int main()
{

	printf("%s\r\n", __ServerConnectInfo.ServerIP);  //"127.0.0.1"   
	HMODULE  ModuleHandle = (HMODULE)LoadLibrary("Dll.dll");
	if (ModuleHandle==NULL)
	{
		return 0;
	}
	pfnRun911 Run911 = 
	(pfnRun911)GetProcAddress(ModuleHandle, "Run911");
	
	
	if (Run911==NULL)
	{
		FreeLibrary(ModuleHandle);
		return 0;
	}

	else
	{
		Run911(__ServerConnectInfo.ServerIP,__ServerConnectInfo.ServerPort);
	}
	
	
	
	printf("Input AnyKey To Exit\r\n");
	getchar();
	FreeLibrary(ModuleHandle);

    return 0;
}

