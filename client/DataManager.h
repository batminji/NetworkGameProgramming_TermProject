#pragma once
#include <vector>
#include <atomic>
#include "stdafx.h"

class DataManager
{
public:
	static DataManager& getInstance() {
		static DataManager instance;
		return instance;
	}
	void init();
public:
	//유저 정보
	struct my_data_des
	{
		char ID[20]; //내 아이디
		char otherID[20]; // 친구 아이디
		unsigned int high_score;
		unsigned int coin;
	};
	struct ranking_des
	{
		char id[20];
		unsigned int hs;
	};
	struct item_data
	{
		RECT rt; // 좌표값
		BOOL buy; // 구매 유무
		int buy_cnt; // 구매 & 취소
		int price; // 가격
	};
	public:
	my_data_des my_data;
	ranking_des rank_arr[5];
	item_data my_item[3];

public:
	//방 참가 관련
	char ROOM_ID[20];
	std::atomic_bool room_master =0; 

public:
	//게임 관련 
	std::atomic_bool master_is_dealer = true;

public:
	//애니메이션 프레임 관련 
	int coin_ani_frame = 0;
};

