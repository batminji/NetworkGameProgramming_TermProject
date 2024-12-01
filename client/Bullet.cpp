#include "Bullet.h"

bullet::bullet(short x, short y, short t, short j)
{
	xy[0] = x, xy[1] = y;
	type = t;
	job = j;
	Kirby_pink_bullet = &ResourceManager::getInstance().Kirby_pink_bullet;
	Kirby_blue_bullet = &ResourceManager::getInstance().Kirby_blue_bullet;
	Kirby_pink_skill_bullet = &ResourceManager::getInstance().Kirby_pink_skill_bullet;
	Monster_bullet = &ResourceManager::getInstance().monster_bullet;
}

void bullet::render(HDC mdc)
{
	switch (job) {
	case 1: // Dealer
		switch (type) {
		case 1:
			Kirby_blue_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 10, 40, 20, basic_bullet_frame, 0, 270, 160, RGB(255, 0, 255));
			break;
		case 2:
			Kirby_pink_skill_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 15, 40, 30, skill_bullet_frame, 0, 260, 260, RGB(255, 0, 255));
			break;
		}
		break;
	case 2: // Healer
		switch (type) {
		case 1:
			Kirby_pink_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 10, 40, 20, basic_bullet_frame, 0, 270, 160, RGB(255, 0, 255));
			break;
		case 2:
			Kirby_pink_skill_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 15, 40, 30, skill_bullet_frame, 0, 260, 260, RGB(255, 0, 255));
			break;
		}
		break;
	case 3: // Monster
		Monster_bullet->TransparentBlt(mdc, xy[0], xy[1], 10, 10, 0, 0, 30, 30, RGB(255, 0, 255));
		break;
	}
}

void bullet::update()
{
	basic_bullet_frame += 270;
	if (basic_bullet_frame == 1350)basic_bullet_frame = 0;
	skill_bullet_frame += 260;
	if (skill_bullet_frame == 1300)skill_bullet_frame = 0;
}
