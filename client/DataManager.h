#pragma once
#include <vector>
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
		unsigned int high_score;
		unsigned int coin;
	};
	struct ranking_des
	{
		char id[20];
		unsigned int hs;
	};
	public:
	my_data_des my_data;
	std::vector<ranking_des> rank_data;

public:
	//�� ���� ����
	char ROOM_ID[20];
	bool room_master;


};

