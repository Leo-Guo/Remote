#pragma once
#include <vector>
#include <iostream>
using namespace std;

enum
{
	CLIENT_LOGIN,				// ���߰�
	CLIENT_GET_OUT,
	CLIENT_GO_ON,
	CLIENT_INSTANT_MESSAGE_REQUEST,
	CLIENT_INSTANT_MESSAGE_REPLY,
	CLIENT_INSTANT_MESSAGE_COMPLETE,
	CLIENT_SHUT_DOWN_REQUEST,
	CLIENT_SHUT_DOWN_REPLY,
	CLIENT_CMD_MANAGER_REQUIRE,
	CLIENT_CMD_MANAGER_REPLY,
	CLIENT_PROCESS_MANAGER_REQUIRE,
	CLIENT_PROCESS_MANAGER_REPLY,
	CLIENT_PROCESS_REFRESH_REQUIRE,
	CLIENT_PROCESS_RING3_KILL_REQUIRE,
	CLIENT_PROCESS_RING0_KILL_REQUIRE,
	CLIENT_PROCESS_KILL_REPLY,
	CLIENT_FILE_MANAGER_REQUIRE,
	CLIENT_FILE_MANAGER_REPLY,
	CLIENT_FILE_LIST_REQUIRE,
	CLIENT_FILE_LIST_REPLY,
	CLIENT_WINDOW_MANAGER_REQUIRE,
	CLIENT_WINDOW_MANAGER_REPLY,
	CLIENT_WINDOW_REFRESH_REQUIRE,
	CLIENT_REMOTE_CONTROLL_REQUIRE,
	CLIENT_REMOTE_CONTROLL_REPLY,
	CLIENT_REMOTE_CONTROLL_FIRST_SCREEN,
	CLIENT_REMOTE_CONTROLL_NEXT_SCREEN,
	CLIENT_REMOTE_CONTROLL_CONTROL,
	CLIENT_REMOTE_CONTROLL_BLOCK_INPUT,
	CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUIRE,
	CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY,
	CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REQUIRE,
	CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REPLY,
	CLIENT_SERVICE_MANAGER_REQUIRE,
	CLIENT_SERVICE_MANAGER_REPLY,
	CLIENT_SERVICE_CONFIG_REQUIRE,
	CLIENT_REGISTER_MANAGER_REQUIRE,
	CLIENT_REGISTER_MANAGER_REPLY
};
typedef BOOL(*f1)();   
typedef char* (*f2)(ULONG&);
typedef BOOL(*f3)(char*);
typedef BOOL (*f4)(PULONG,ULONG);
typedef BOOL(*f5)(char*, ULONG);
