#include "Player.h"

Player::Player()
{
	if (job == 1) {
		kirby_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_HIT));
		kirby_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_ZOMBIE));
	}
	else {
		kirby_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_HIT));
		kirby_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_ZOMBIE));

	}
}

void Player::render(HDC mdc)
{
	if (zombie_cnt == 0) {
		RECT kirby_rt = { x - 25, y - 25, x + 25, y + 25 };
		kirby_fly.TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, 0, 0, 25, 25, RGB(0, 255, 0));
	}
	else if (zombie_cnt < 8) {
		RECT kirby_rt = { x - 25,y - 25,x + 25,y + 25 };
		kirby_hit.TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
	}
	else {
		RECT kirby_rt = { x - 25,y - 25,x + 25, y + 25 };
		kirby_zombie.TransparentBlt(mdc, kirby_rt.left, kirby_rt.top, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
	}
}
