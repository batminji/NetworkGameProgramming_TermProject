#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <algorithm>
#include <random>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <atlImage.h>
#include "..\Kirby\resource.h"

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

