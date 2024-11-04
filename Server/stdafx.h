#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <winsock2.h> // 절대 순서를 바꾸지 말 것..
#include <windows.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <algorithm>
#include <random>
#include <thread>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <stdio.h> 
#include <stdlib.h>
#include <atlImage.h>
#include "..\Kirby\resource.h"
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

// enum
enum PACKET // 패킷 타입 정의.
{
	// C to S
	CS_LOGIN_PACKET,
	CS_MOVE_PACKET,
	CS_KEY_INPUT_PACKET,
	CS_ROOM_STATE_PACKET,

	// S to C
	SC_LOGIN_RESULT_PACKET,
	SC_ROOM_CHANGE_PACKET,
	SC_PLAYER_MOVE_PACKET,
	SC_PLAYER_STATE_CHANGE_PACKET,
	SC_OBJECT_MOVE_PACKET,
	SC_OBJECT_CHANGE_PACKET
};

enum OTYPE // 오브젝트 타입 정의.
{ 
	ENEMY, 
	ENEMY_BULLETS, 
	MISSAIL, 
	MY_BULLET, 
	ITEM 
};

// 상수
constexpr int ID_LEN = 20;
constexpr int MAX_OBJ_NUM = 100; // todo: 여기 상의
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512; // todo: 여기 상의

// 소켓 오류 관련
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[오류] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
