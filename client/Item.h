#pragma once
#include "stdafx.h"
#include "ResourceManager.h"
#include "DataManager.h"
class Item
{
public:
	OTYPE type;
	CImage* coin_bmp;
	CImage* dual_bmp;
	CImage* magnet_bmp;
public:
	unsigned short x;
	unsigned short y;
public :
	Item(OTYPE otype, const unsigned short ax, const unsigned short ay);
	Item();
public:
	void render(HDC mdc) ;

};

