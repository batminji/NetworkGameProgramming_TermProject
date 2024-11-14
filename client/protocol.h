#pragma once
#include "stdafx.h"
#pragma pack(push, 1)

// client to server

struct CS_LOGIN_PACKET
{
	unsigned short size;
	PACKET type;
	char id[ID_LEN];
};

struct CS_MOVE_PACKET // �÷��̾� ���콺 ��ġ ����
{
	unsigned short size;
	PACKET type;
	unsigned short y; // x���� �������� �ʾƵ� ��
};

struct CS_KEY_INPUT_PACKET // ��ų �� Ű �Է��� ���� ��
{
	unsigned short size;
	PACKET type;
	unsigned char key; // todo: �ڷ��� �������� ���
};

struct CS_JOIN_ROOM_PACKET
{
	unsigned short size;
	PACKET type;
	char id[ID_LEN];
};

struct CS_ROOM_STATE_PACKET // �� ������ �����ϴ� ��Ŷ
{
	unsigned short size;
	PACKET type;
	bool isPlaying;
	bool isQuit;
};

// server to client

struct SC_LOGIN_RESULT_PACKET
{
	unsigned short size;
	PACKET type;
	bool success; // True�� �� �α��� ����
};

struct SC_ROOM_CHANGE_PACKET // �� ������ ����Ǹ�
{
	unsigned short size;
	PACKET type;
	char other_pl[ID_LEN]; // ģ�� �÷��̾� �̸�
	bool isPlaying; // �����ߴ���
	bool isDealer; // ���� �����ΰ�?
};

struct SC_PLAYER_MOVE_PACKET // ĳ���� �̵�
{
	unsigned short size;
	PACKET type;
	unsigned short this_y; // ������ġ
	unsigned short other_y; // ģ�� ��ġ
};

struct SC_PLAYER_STATE_CHANGE_PACKET
{
	unsigned short size;
	PACKET type;
	unsigned short hp; // �̰ɷ� �������� �Ǵ��ϵ���
	bool isHit; // �¾Ҵ°�?
	bool skillEnd; // ��ų �������� true
};

struct SC_OBJECT_MOVE_PACKET // ������Ʈ �̵�
{
	unsigned short size;
	PACKET type;
	OTYPE obj_type; // � ������Ʈ�� �̵��ΰ�
	unsigned short number; // ����
	unsigned short objs_x[MAX_OBJ_NUM];
	unsigned short objs_y[MAX_OBJ_NUM];
};

struct SC_OBJECT_CHANGE_PACKET // ������Ʈ ���º���
{
	unsigned short size;
	PACKET type;
	OTYPE obj_type;
	bool isRemove; // �����Ǿ��°� Ȥ�� �׾��°�?
};