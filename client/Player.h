#pragma once
#include "stdafx.h"
#include "ResourceManager.h"

static int Kirby_frame = 0;

class Player
{
	 
 public :
	 CImage* kirby_pink_fly;
	 CImage* kirby_pink_hit;
	 CImage* kirby_pink_zombie;

	 CImage* kirby_blue_fly;
	 CImage* kirby_blue_hit;
	 CImage* kirby_blue_zombie;

	 
	
	short job;
	bool room;

	bool inGame;

	short x = 675;
	unsigned short y = 300;
	
	bool skill = FALSE;
	short skill_cnt = 0;
	short zombie_cnt;

	bool die;
	bool who_is_me;
	bool equipments[4];

public: 
	SOCKET* p_sock;

public:

	Player();
	Player(short job, bool who_is_me);
	Player(const Player& other);

public:
	
	void render(HDC mdc);
	void update();
};

