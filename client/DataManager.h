#pragma once
class DataManager
{
public:
	static DataManager& getInstance() {
		static DataManager instance;
		return instance;
	}
	void init();
public:
	struct my_data_des
	{
		char ID[20]; //내 아이디
		unsigned int high_score;
		unsigned int coin;
	};
	struct ranking_des
	{
		char id1[20];
		unsigned int hs1;
		char id2[20];
		unsigned int hs2;
		char id3[20];
		unsigned int hs3;
		char id4[20];
		unsigned int hs4;
		char id5[20];
		unsigned int hs5;
	};
	public:
	my_data_des my_data;
	ranking_des rank_data;



};

