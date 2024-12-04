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
	//���� ����
	struct my_data_des
	{
		char ID[20]; //�� ���̵�
		char otherID[20]; // ģ�� ���̵�
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
		RECT rt; // ��ǥ��
		BOOL buy; // ���� ����
		int buy_cnt; // ���� & ���
		int price; // ����
	};
	public:
	my_data_des my_data;
	ranking_des rank_arr[5];
	item_data my_item[3];

public:
	//�� ���� ����
	char ROOM_ID[20];
	std::atomic_bool room_master =0; 

public:
	//���� ���� 
	std::atomic_bool master_is_dealer = true;

public:
	//�ִϸ��̼� ������ ���� 
	int coin_ani_frame = 0;
};

