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
	struct My
	{
		char ID[20]; //내 아이디
		unsigned int high_score;
		unsigned int coin;
	};


};

