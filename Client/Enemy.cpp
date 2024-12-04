#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(int arg_type,const unsigned short arg_x, const unsigned short arg_y, const unsigned short arg_hp)
{
	type             = arg_type;
	x               =  static_cast<short>(arg_x);
	y               =  static_cast<short>(arg_y);
	hp               = static_cast<short>(arg_hp);
	ani = 0;
	width = 84, height = 96;
	if (type == 0) width = 84, height = 96;
	else if (type == 1 || type == 2)width = 90, height = 130;
	else if (type == 3)width = 100, height = 120;
	else if (type == 4)width = 256, height = 224;
	


	type == 4 ? frame = 6 : frame = 3;

	

	monster[0] = &ResourceManager::getInstance().m[0];
	monster[1] = &ResourceManager::getInstance().m[1];
	monster[2] = &ResourceManager::getInstance().m[2];
	monster[3] = &ResourceManager::getInstance().m[3];
	monster[4] = &ResourceManager::getInstance().m[4];
	monster_create = &ResourceManager::getInstance().monster_create;
	monster_die = &ResourceManager::getInstance().monster_die;
	hp_bar = &ResourceManager::getInstance().hp_bar;
	hp_empty = &ResourceManager::getInstance().hp_empty;
}

void Enemy::render(HDC mdc)
{
	

	monster[type]->TransparentBlt(mdc, x, y, width, height, (width/2)*ani, 0, (width) / 2, (height) / 2, RGB(255, 0, 255));
	if (type > 0) {
		num = width;
		hp_empty->TransparentBlt(mdc, x - 30, y - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
		int hp_copy = min(((double)hp / (double)50 * 224) * ((double)num / 160) + 1, 224 * ((double)num / 160));
		int hp_socrce = min(224, ((double)hp / (double)50) * 224);
		hp_bar->TransparentBlt(mdc, x - 30, y - 22, hp_copy, 22, 0, 0,hp_socrce, 22, RGB(0, 255, 0));
	}

	
}

void Enemy::update()
{
	ani = (ani + 1) %frame;
}
