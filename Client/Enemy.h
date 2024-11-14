#pragma once
#include "stdafx.h"
#include "ResourceManager.h"
class Enemy
{
public :
	short type;
	 RECT rt;
	short hp;
	short cnt;
	short frame;
	short attack_type;
public:
	CImage bitmap;
public: 
	Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame, short arg_attack_type);

};

