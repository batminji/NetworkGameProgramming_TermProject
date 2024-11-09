#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

class Player
{

 public :
	//SOCKET socket;

	 CImage* kirby_fly;
	 CImage* kirby_die;
	 CImage* kirby_hit;
	 CImage* kirby_zombie;
	
	short job;
	
	std::string	id;
	
	bool inGame;

	short x = 675;
	
	short y;
	
	bool skill;

	short skill_cnt;

	short zombie_cnt;

	bool die;

	bool equipments[4];

	vector <bullet> bullets;

public:

	Player();

public:
	
	void render(HDC mdc);
};

