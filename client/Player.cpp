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

Player::Player(short j, std::string i, bool w)
{
	kirby_pink_fly = &ResourceManager::getInstance().Kirby_pink_fly;
	kirby_pink_die = &ResourceManager::getInstance().Kirby_pink_die;
	kirby_pink_hit = &ResourceManager::getInstance().Kirby_pink_hit;
	kirby_pink_zombie = &ResourceManager::getInstance().Kirby_pink_zombie;

	kirby_blue_fly = &ResourceManager::getInstance().Kirby_blue_fly;
	// kirby_blue_die = &ResourceManager::getInstance().Kirby_blue_die;
	kirby_blue_hit = &ResourceManager::getInstance().Kirby_blue_hit;
	kirby_blue_zombie = &ResourceManager::getInstance().Kirby_blue_zombie;
	job = j;
	id = i;
	who_is_me = w;
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

	if (zombie_cnt > 0) {
		if (zombie_cnt == 1)zombie_cnt++;
		if (zombie_cnt > 30)zombie_cnt = 0;
	}
	
	//플레이어 좌표 받기
	/*char recvBuf[BUFSIZE];
	int recvLen = recv(*m_sock, recvBuf, sizeof(SC_PLAYER_MOVE_PACKET), 0);
	if (recvLen <= 0) {
		std::cerr << "플레이어 좌표받기 실패" << std::endl;
		return -1;
	}
	else {
		SC_PLAYER_MOVE_PACKET* resPacket = reinterpret_cast<SC_PLAYER_MOVE_PACKET*>(recvBuf);

	}

	//플레이어 상태 받기
	char recvBuf[BUFSIZE];
	int recvLen = recv(*m_sock, recvBuf, sizeof(SC_PLAYER_STATE_CHANGE_PACKET), 0);
	if (recvLen <= 0) {
		std::cerr << "플레이어 상태받기 실패" << std::endl;
		return -1;
	}
	else {
		SC_PLAYER_STATE_CHANGE_PACKET* resPacket = reinterpret_cast<SC_PLAYER_STATE_CHANGE_PACKET*>(recvBuf);

	}*/

}
