#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame)
{
	type             = arg_type;
	rt               = arg_rt;
	hp               = arg_hp;
	cnt              = arg_cnt;
	frame            = arg_frame;

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
	if (cnt < 0) {
		monster_create->TransparentBlt(mdc, rt.left, rt.top, (rt.right - rt.left), (rt.bottom - rt.top), 33 * (6 + (cnt / 3)), 0, 33, 33, RGB(255, 0, 255));
	}
	else {
		monster[type]->TransparentBlt(mdc, rt.left, rt.top, (rt.right - rt.left), (rt.bottom - rt.top), ((rt.right - rt.left) / 2) * (ani % (frame)), 0, (rt.right - rt.left) / 2, (rt.bottom - rt.top) / 2, RGB(255, 0, 255));
		if (type > 0) {
			num = rt.right - rt.left;
			hp_empty->TransparentBlt(mdc, rt.left - 30, rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
			hp_bar->TransparentBlt(mdc, rt.left - 30, rt.top - 22, (((double)hp / (double)50 * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)hp / (double)50) * 224, 22, RGB(0, 255, 0)); 
		}
	}

	// 일단 생성 되는 부분을 안 넣는다고 했을 때
	monster[type]->TransparentBlt(mdc, rt.left, rt.top, (rt.right - rt.left), (rt.bottom - rt.top), ((rt.right - rt.left) / 2) * (ani % (frame)), 0, (rt.right - rt.left) / 2, (rt.bottom - rt.top) / 2, RGB(255, 0, 255));
	if (type > 0) {
		num = rt.right - rt.left;
		hp_empty->TransparentBlt(mdc, rt.left - 30, rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
		hp_bar->TransparentBlt(mdc, rt.left - 30, rt.top - 22, (((double)hp / (double)50 * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)hp / (double)50) * 224, 22, RGB(0, 255, 0));
	}

	// 이펙트를 여기에 넣나요?
}

void Enemy::update()
{

}
