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
	unsigned short x;
	unsigned short y;
public :
	Item(const unsigned short ax, const unsigned short ay);
	Item();
public:
	virtual void render(HDC mdc) = 0 ;

};

class coin :public Item 
{
public:
	coin(const unsigned short ax, const unsigned short ay) : Item(ax, ay) {} 
	void render(HDC mdc) override;

};

class margnet : 
	public Item {
public:
		margnet(const unsigned short ax, const unsigned short ay) : Item(ax, ay) {}

	void render(HDC mdc) override;

};

class dual : // ���ݷ� �ι� ������
	public Item {
public:
	dual(const unsigned short ax, const unsigned short ay) : Item(ax, ay) {}
	void render(HDC mdc) override;

};