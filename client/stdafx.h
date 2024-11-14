#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <winsock2.h> 
#include <windows.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <algorithm>
#include <string>
#include <random>
#include <thread>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <atlImage.h>
#include "resource.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크

using namespace std;
using namespace FMOD;

//씬
#define TITLE_SCENE 0
#define LOBBY_SCENE 1
#define ROOM_SCENE 2
#define PLAY_SCENE 3
#define CARTOON_SCENE 4

enum PACKET // 패킷 타입 정의.
{
	// C to S
	CS_LOGIN,
	CS_MOVE,
	CS_KEY_INPUT,
	CS_JOIN_ROOM,
	CS_ROOM_STATE,

	// S to C
	SC_LOGIN_RESULT,
	SC_ROOM_CHANGE,
	SC_PLAYER_MOVE,
	SC_PLAYER_STATE_CHANGE,
	SC_OBJECT_MOVE,
	SC_OBJECT_CHANGE
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

//// 소켓 오류 관련
//void err_quit(const char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(char*)&lpMsgBuf, 0, NULL);
//	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
//	LocalFree(lpMsgBuf);
//	exit(1);
//}
//
//// 소켓 함수 오류 출력
//void err_display(const char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(char*)&lpMsgBuf, 0, NULL);
//	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}
//
//// 소켓 함수 오류 출력
//void err_display(int errcode)
//{
//	LPVOID lpMsgBuf;
//	FormatMessageA(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, errcode,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(char*)&lpMsgBuf, 0, NULL);
//	printf("[오류] %s\n", (char*)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}
//
//void client_info(SOCKET s, std::string str)
//{
//	// 클라이언트 주소 정보 확인
//	sockaddr_in clientAddr;
//	int addrLen = sizeof(clientAddr);
//	getpeername(s, (sockaddr*)&clientAddr, &addrLen);
//
//	// 클라이언트 정보 출력
//	std::cout << "Client connected - ID: " << str
//		<< ", IP: " << inet_ntoa(clientAddr.sin_addr)
//		<< ", Port: " << ntohs(clientAddr.sin_port)
//		<< ", Socket: " << s << std::endl;
//}