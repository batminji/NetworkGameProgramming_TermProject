#pragma once
#include "stdafx.h"
#include "ResourceManager.h"
class Item
{
public:
	CImage* coin_bmp;
	CImage* dual_bmp;
	CImage* magnet_bmp;
public:
	short x;
	short y;
public :
	Item(short ax, short ay);
	Item();
public:
	virtual void render(HDC mdc) = 0 ;

};

class coin :
	public Item {

	void render(HDC mdc) override;

};

class margnet : 
	public Item {

	void render(HDC mdc) override;

};

class dual : // 공격력 두배 아이템
	public Item {

	void render(HDC mdc) override;

};