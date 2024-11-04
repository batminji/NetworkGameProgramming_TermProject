#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <winsock2.h> // ���� ������ �ٲ��� �� ��..
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
#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

// enum
enum PACKET // ��Ŷ Ÿ�� ����.
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

enum OTYPE // ������Ʈ Ÿ�� ����.
{ 
	ENEMY, 
	ENEMY_BULLETS, 
	MISSAIL, 
	MY_BULLET, 
	ITEM 
};

// ���
constexpr int ID_LEN = 20;
constexpr int MAX_OBJ_NUM = 100; // todo: ���� ����
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512; // todo: ���� ����

// ���� ���� ����
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

// ���� �Լ� ���� ���
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

// ���� �Լ� ���� ���
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[����] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
