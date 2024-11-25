#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

static int ani;
static int num;

class Enemy
{
public :
	short type; //0:���� 1~3:�̵� 4:��
	RECT rt;
	short hp;
	short cnt; //ź������ī��Ʈ
	short frame;
public:
	CImage* monster[5];
	CImage* monster_create;
	CImage* monster_die;
	CImage* hp_bar;
	CImage* hp_empty;

public: 
	Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame);
	void render(HDC mdc);
	void update();
};