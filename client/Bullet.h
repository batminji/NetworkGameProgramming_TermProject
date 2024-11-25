#pragma once
#include "stdafx.h"
#include "ResourceManager.h"

static int basic_bullet_frame = 0;
static int skill_bullet_frame = 0;

class bullet
{
	CImage* Kirby_pink_bullet;
	CImage* Kirby_blue_bullet;
	CImage* Kirby_pink_skill_bullet;
	CImage* Monster_bullet;

	short job; // 1 : Dealer 2 : Healer 3 : Monster
	short type; // 1 : Basic bullet 2 : Skill Bullet
	short xy[2]; // 그리는 위치
public:
	bullet(short x, short y, short t, short j); // x좌표, y좌표, x방향 속도, y방향 속도, 총알 타입, 누구의 총알인지
	void render(HDC mdc);
	void update();
};

