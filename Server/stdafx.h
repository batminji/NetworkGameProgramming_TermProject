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
#include <unordered_map>
#include <fstream>
#include <chrono>
#include <array>
#include <mutex>
#include <concurrent_priority_queue.h>
#include <concurrent_queue.h>
#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

// enum
enum PACKET // ��Ŷ Ÿ�� ����.
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
	SC_OBJECT_CHANGE,
	SC_RANKING
};

enum OTYPE // ������Ʈ Ÿ�� ����.
{
	ENEMY,
	ENEMY_BULLETS,
	MISSAIL,
	P1_BULLET,
	P2_BULLET,
	P1_SKILLBULLET,
	P2_SKILLBULLET,
	ITEM 
};

// ���
constexpr int ID_LEN = 20;
constexpr int MAX_OBJ_NUM = 100; // todo: ���� ����
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512; // todo: ���� ����

void SERVER_err_quit(const char* msg);
void SERVER_err_display(const char* msg);
void SERVER_err_display(int errcode);
void client_info(SOCKET s, std::string str);