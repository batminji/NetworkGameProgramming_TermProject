#pragma once
#include "stdafx.h"

class Item
{

public:
	short xy[2];

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

class dual : // ���ݷ� �ι� ������
	public Item {

	void render(HDC mdc) override;

};