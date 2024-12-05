#include "Player.h"

Player::Player()
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;

	

	job = 1;
}

Player::Player(short j, bool w)
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;

	
	job = j;
	who_is_me = w;
	room = FALSE;
}

Player::Player(const Player& other)
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;

	

	job = other.job;
	who_is_me = other.who_is_me;
	
}

void Player::render(HDC mdc)
{
	switch (job) {
	case 1: 
		//dealer
		if (zombie_cnt == 0) kirby_pink_fly->TransparentBlt(mdc, x - 25, y - 25, 50, 50, Kirby_frame, 0, 25, 25, RGB(0, 255, 0));
		else if (zombie_cnt < 8) kirby_pink_hit->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
		else kirby_pink_zombie->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
		break;
	case 2: // Healer
		if (zombie_cnt == 0) kirby_blue_fly->TransparentBlt(mdc, x - 25, y - 25, 50, 50, Kirby_frame, 0, 25, 25, RGB(0, 255, 0));
		else if (zombie_cnt < 8) kirby_blue_hit->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
		else kirby_blue_zombie->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
		break;
	default:
		break;
	}
}

void Player::update()
{
	Kirby_frame = (Kirby_frame + 25) % 200;

	if (zombie_cnt > 0) {
		if (zombie_cnt == 1)zombie_cnt++;
		if (zombie_cnt > 30)zombie_cnt = 0;
	}
}
