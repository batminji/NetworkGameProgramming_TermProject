#pragma once
#include "stdafx.h"

class ResourceManager
{
public:
	// ResourceManager의 인스턴스를 반환
	static ResourceManager& getInstance()
	{
		static ResourceManager instance;
		return instance;
	}


	void init(HINSTANCE g_hInst);
	void destroy();


	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	// 그래픽 리소스들 선언
	CImage Kirby_shield;

	CImage Kirby_blue_bullet;
	
	CImage Kirby_pink_idle_left;
	CImage Kirby_pink_idle_right;
	CImage Kirby_blue_idle_left;
	CImage Kirby_blue_idle_right;

	CImage Room_screen;
	CImage Dealer_check;
	CImage Healer_check;
	CImage Room_screen_bg;

	CImage Kirby_blue_fly;
	CImage Kirby_blue_hit;
	CImage Kirby_blue_zombie;
	CImage Kirby_blue_die;

	CImage start_screen;
	CImage title;
	CImage cartoon[6];
	CImage cartoon_bg;
	CImage main_screen;
	CImage start_button;
	CImage item_check;
	CImage click_cursor;
	CImage game_bg;
	CImage Kirby_pink_fly;
	CImage Kirby_pink_die;
	CImage Kirby_pink_hit;
	CImage Kirby_pink_zombie;
	CImage heart;
	CImage Kirby_pink_bullet;
	CImage game_bg2;
	CImage skill;
	CImage skill_color;
	CImage Kirby_pink_skill_bullet;
	CImage m[5];
	CImage monster_bullet;
	CImage number;
	CImage hp_bar, hp_empty;
	CImage skill_effects;
	CImage game_over;
	CImage over_number;
	CImage warning;
	CImage missile_explosion;
	CImage missile_bmp;
	CImage coin_bmp;
	CImage monster_create;
	CImage monster_die;
	CImage hit_effect;
	CImage rezero_bg;
	CImage item[4];
	CImage dual_bmp;
	CImage magnet_bmp;

	// 사운드 리소스들 선언
 // &ResourceManager::getinstance().ssystem -> 이런식으로 사용해야함
	System* ssystem;
	Sound* title_bgm;
	Sound* main_bgm;
	Sound* cartoon_bgm;
	Sound* battle_bgm;
	Sound* click_sound;
	Sound* damage_sound;
	Sound* gameover_sound;
	Sound* shop_sound;
	Sound* skill_sound;
	Sound* cant_shop_sound;
	Sound* shop_cancle_sound;
	Sound* explosion_sound;
	Sound* warning_sound;
	Sound* coin_sound;
	Channel* channel = 0;
	FMOD_RESULT result;
	void* extradriverdata = 0;

private:

	ResourceManager() : initialized(false) {}

	bool initialized; 
};
