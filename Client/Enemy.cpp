#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame, short arg_attack_type)
{
	type             = arg_type;
	rt               = arg_rt;
	hp               = arg_hp;
	cnt              = arg_cnt;
	frame            = arg_frame;
	attack_type       = arg_attack_type;

	monster[0] = &ResourceManager::getInstance().m[0];
	monster[1] = &ResourceManager::getInstance().m[1];
	monster[2] = &ResourceManager::getInstance().m[2];
	monster[3] = &ResourceManager::getInstance().m[3];
	monster[4] = &ResourceManager::getInstance().m[4];
	monster_create = &ResourceManager::getInstance().monster_create;
	monster_die = &ResourceManager::getInstance().monster_die;
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
			/*hp_empty.TransparentBlt(mdc, paint_enemy.rt.left - 30, paint_enemy.rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
			hp_bar.TransparentBlt(mdc, paint_enemy.rt.left - 30, paint_enemy.rt.top - 22, abs((((double)paint_enemy.hp / (double)full_hp[paint_enemy.type]) * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)paint_enemy.hp / (double)full_hp[paint_enemy.type]) * 224, 22, RGB(0, 255, 0));*/
		}
	}

	// 이펙트를 여기에 넣나요?

	// 탄막
	for (int i = 0; i < bullets.size(); ++i) bullets[i].render(mdc);
}

void Enemy::update()
{

}
