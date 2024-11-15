#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

static int ani;
static int num;

class Enemy
{
public :
	short type;
	RECT rt;
	short hp;
	short cnt;
	short frame;
	short attack_type;
	vector <bullet> bullets;
public:
	CImage* monster[5];
	CImage* monster_create;
	CImage* monster_die;
public: 
	Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame, short arg_attack_type);
	void render(HDC mdc);
	void update();
};

