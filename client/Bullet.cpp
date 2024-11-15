#include "Bullet.h"

bullet::bullet(short x, short y, short t)
{
	xy[0] = x, xy[1] = y;
	y = type;
	Basic_bullet = &ResourceManager::getInstance().basic_bullet;
	Skill_bullet = &ResourceManager::getInstance().skill_bullet;
	Monster_bullet = &ResourceManager::getInstance().monster_bullet;
}

void bullet::render(HDC mdc)
{
	switch (type) {
	case 1: // ±×³É ÃÑ¾Ë
		Basic_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 10, 40, 20, basic_bullet_frame, 0, 270, 160, RGB(255, 0, 255));
		break;
	case 2: // ½ºÅ³ ÃÑ¾Ë
		Skill_bullet->TransparentBlt(mdc, xy[0] - 20, xy[1] - 15, 40, 30, skill_bullet_frame, 0, 260, 260, RGB(255, 0, 255));
		break;
	case 3: // Àû ÃÑ¾Ë
		Monster_bullet->TransparentBlt(mdc, xy[0], xy[1], 10, 10, 0, 0, 30, 30, RGB(255, 0, 255));
	}
}

void bullet::update()
{
	basic_bullet_frame += 270;
	if (basic_bullet_frame == 1350)basic_bullet_frame = 0;
	skill_bullet_frame += 260;
	if (skill_bullet_frame == 1300)skill_bullet_frame = 0;

	xy[0] -= 20;
}
