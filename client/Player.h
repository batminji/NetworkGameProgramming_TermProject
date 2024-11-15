#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

static int Kirby_frame = 0;

class Player
{
	 
 public :
	 CImage* kirby_pink_fly;
	 CImage* kirby_pink_die;
	 CImage* kirby_pink_hit;
	 CImage* kirby_pink_zombie;

	 CImage* kirby_blue_fly;
	 CImage* kirby_blue_die;
	 CImage* kirby_blue_hit;
	 CImage* kirby_blue_zombie;
	
	short job;
	bool room;
	
	std::string	id;
	
	bool inGame;

	short x = 675;
	short y = 300;
	
	bool skill = FALSE;
	short skill_cnt = 0;
	short zombie_cnt;

	bool die;
	bool who_is_me;
	bool equipments[4];

	vector <bullet> basic_bullets;
	vector <bullet> skill_bullets;

public:

	Player();
	Player(short job, string id, bool who_is_me);

public:
	
	void render(HDC mdc);
	void update();
};

