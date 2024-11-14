#include "Player.h"

Player::Player()
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_die = &ResourceManager::getInstance().Kirby_pink_die;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	// kirby_blue_die = &ResourceManager::getInstance().Kirby_blue_die;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;

	job = 1;
}

Player::Player(short job, std::string id)
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_die = &ResourceManager::getInstance().Kirby_pink_die;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	// kirby_blue_die = &ResourceManager::getInstance().Kirby_blue_die;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;
	job = job;
	id = id;
}

void Player::render(HDC mdc)
{
	switch (job) {
	case 1: // dealer
		for (int i = 0; i < basic_bullets.size(); ++i) basic_bullets[i].render(mdc);
		for (int i = 0; i < skill_bullets.size(); ++i) skill_bullets[i].render(mdc);
		if (zombie_cnt == 0) kirby_pink_fly->TransparentBlt(mdc, x - 25, y - 25, 50, 50, Kirby_frame, 0, 25, 25, RGB(0, 255, 0));
		else if (zombie_cnt < 8) kirby_pink_hit->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
		else kirby_pink_zombie->TransparentBlt(mdc, x - 25, y - 25, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
		break;
	case 2: // Healer
		for (int i = 0; i < basic_bullets.size(); ++i) basic_bullets[i].render(mdc);
		for (int i = 0; i < skill_bullets.size(); ++i) skill_bullets[i].render(mdc);
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

	//플레이어 좌표받아오기
	
}
