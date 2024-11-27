#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(const OTYPE arg_type,const unsigned short arg_x, const unsigned short arg_y, const unsigned short arg_hp)
{
	type             = static_cast<short>(arg_type);
	x               =  static_cast<short>(arg_x);
	y               =  static_cast<short>(arg_y);
	hp               = static_cast<short>(arg_hp);

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
	/*if (cnt < 0) {
		monster_create->TransparentBlt(mdc, x, y, (rt.right - rt.left), (rt.bottom - rt.top), 33 * (6 + (cnt / 3)), 0, 33, 33, RGB(255, 0, 255));
	}
	else {
		monster[type]->TransparentBlt(mdc, x,y, (rt.right - rt.left), (rt.bottom - rt.top), ((rt.right - rt.left) / 2) * (ani % (frame)), 0, (rt.right - rt.left) / 2, (rt.bottom - rt.top) / 2, RGB(255, 0, 255));
		if (type > 0) {
			num = rt.right - rt.left;
			hp_empty->TransparentBlt(mdc, rt.left - 30, rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
			hp_bar->TransparentBlt(mdc, rt.left - 30, rt.top - 22, (((double)hp / (double)50 * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)hp / (double)50) * 224, 22, RGB(0, 255, 0)); 
		}
	}*/

	// 일단 생성 되는 부분을 안 넣는다고 했을 때
	monster[type]->TransparentBlt(mdc, x, y, x+width, y+height, (width / 2) * ani, 0, (width) / 2, (height) / 2, RGB(255, 0, 255));
	/*if (type > 0) {
		num = rt.right - rt.left;
		hp_empty->TransparentBlt(mdc, rt.left - 30, rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
		hp_bar->TransparentBlt(mdc, rt.left - 30, rt.top - 22, (((double)hp / (double)50 * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)hp / (double)50) * 224, 22, RGB(0, 255, 0));
	}*/

	// 이펙트를 여기에 넣나요?
}

void Enemy::update()
{
	ani = (ani + 1) % frame;
}
