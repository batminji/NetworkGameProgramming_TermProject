#include "Player.h"

Player::Player()
{
	kirby_fly = &ResourceManager::getInstance().kirby_fly;
	kirby_die = &ResourceManager::getInstance().kirby_die;
	kirby_hit = &ResourceManager::getInstance().kirby_hit;
	kirby_zombie = &ResourceManager::getInstance().kirby_zombie;
}

void Player::render(HDC mdc)
{
	if (zombie_cnt == 0) {
		RECT kirby_rt = { x - 25, y - 25, x + 25, y + 25 };
		kirby_fly->TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, 0, 0, 25, 25, RGB(0, 255, 0));
	}
	else if (zombie_cnt < 8) {
		RECT kirby_rt = { x - 25,y - 25,x + 25,y + 25 };
		kirby_hit->TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
	}
	else {
		RECT kirby_rt = { x - 25,y - 25,x + 25, y + 25 };
		kirby_zombie->TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
	}
}
