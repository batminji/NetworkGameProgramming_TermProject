#pragma once
#include "stdafx.h"
#include "ResourceManager.h"

static int basic_bullet_frame = 0;
static int skill_bullet_frame = 0;

class bullet
{
	CImage* Basic_bullet;
	CImage* Skill_bullet;
	CImage* Monster_bullet;

	short type;
	short xy[2];
public:
	bullet(short x, short y, short t);
	void render(HDC mdc);
	void update();
};

