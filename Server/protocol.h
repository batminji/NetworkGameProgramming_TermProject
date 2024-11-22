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

struct CS_MOVE_PACKET // 플레이어 마우스 위치 전송
{
	unsigned short size;
	PACKET type;
	unsigned short y; // x값은 전송하지 않아도 됨
	bool keyDown; // 스킬
};

struct CS_JOIN_ROOM_PACKET
{
	unsigned short size;
	PACKET type;
	char id[ID_LEN];
};

struct CS_ROOM_STATE_PACKET // 방 설정을 변경하는 패킷
{
	unsigned short size;
	PACKET type;
	bool isDealer; // 클라이언트가 자기 딜러로 바꾸는 변수
	bool isPlaying;
	bool isQuit;
};

// server to client

struct SC_LOGIN_RESULT_PACKET
{
	unsigned short size;
	PACKET type;
	bool success; // True일 때 로그인 성공
	bool is_new; // 이게 true일때 컷씬 실행해주세요
	// 플레이어 기존 데이터 전송
	unsigned int high_score; 
	unsigned int coin;
};

struct SC_ROOM_CHANGE_PACKET // 방 설정이 변경되면
{
	unsigned short size;
	PACKET type;
	char other_pl[ID_LEN]; // 친구 플레이어 이름
	bool isPlaying; // 시작했는지
	bool isDealer; // 내가 딜러인가?
};

struct SC_PLAYER_MOVE_PACKET // 캐릭터 이동
{
	unsigned short size;
	PACKET type;
	unsigned short this_y; // 나의위치
	unsigned short other_y; // 친구 위치
};

struct SC_PLAYER_STATE_CHANGE_PACKET
{
	unsigned short size;
	PACKET type;
	unsigned short hp; // 이걸로 죽음까지 판단하도록
	bool isHit; // 맞았는가?
	bool skillEnd; // 스킬 끝났을떄 true
};

struct SC_OBJECT_MOVE_PACKET // 오브젝트 이동
{
	unsigned short size;
	PACKET type;
	OTYPE obj_type; // 어떤 오브젝트의 이동인가
	unsigned short number; // 개수
	unsigned short objs_x[MAX_OBJ_NUM];
	unsigned short objs_y[MAX_OBJ_NUM];
};

struct SC_OBJECT_CHANGE_PACKET // 오브젝트 상태변경
{
	unsigned short size;
	PACKET type;
	OTYPE obj_type;
	bool isRemove; // 삭제되었는가 혹은 죽었는가?
};

struct SC_RANKING_PACKET 
{
	unsigned short size;
	PACKET type;
	char id1[ID_LEN];
	unsigned int hs1;
	char id2[ID_LEN];
	unsigned int hs2;
	char id3[ID_LEN];
	unsigned int hs3;
	char id4[ID_LEN];
	unsigned int hs4;
	char id5[ID_LEN];
	unsigned int hs5;
};