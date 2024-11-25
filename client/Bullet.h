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

	short job;
	short type;
	short xy[2];
public:
	bullet(short x, short y, short t, short j); // y ÁÂÇ¥, ÃÑ¾Ë Å¸ÀÔ, ´©±¸ÀÇ ÃÑ¾ËÀÎÁö
	// 1 : DealerÀÇ ÃÑ¾Ë 2 : HealerÀÇ ÃÑ¾Ë 
	void render(HDC mdc);
	void update();
};

