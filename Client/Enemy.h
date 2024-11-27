#pragma once
#include "stdafx.h"
#include "Bullet.h"
#include "ResourceManager.h"

static int ani;
static int num;

class Enemy
{
public :
	int type; //0:���� 1~3:�̵� 4:��
	short x, y;
	short width, height;
	short hp;
	short cnt; //ź������ī��Ʈ
	short frame; //�� �ִϸ��̼��������� ���������...;
	short ani; //����������
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