#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

static int ani;
static int num;

class Enemy
{
public :
	int type; //0:스몰 1~3:미드 4:빅
	short x, y;
	short width, height;
	short hp;
	short cnt; //탄막생성카운트
	short frame; //걍 애니메이션프레임이 몇개인지였음...;
	short ani; //현재프레임
public:
	CImage* monster[5];
	CImage* monster_create;
	CImage* monster_die;
	CImage* hp_bar;
	CImage* hp_empty;

public: 
	Enemy(int arg_type, const unsigned short arg_x, const unsigned short arg_y, const unsigned short arg_hp);
	void render(HDC mdc);
	void update();
};