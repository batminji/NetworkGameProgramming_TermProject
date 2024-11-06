#pragma once
#include "stdafx.h"
class ResourceManager
{
public:
	//graphic resource
	CImage start_screen;
	CImage title;
	CImage cartoon[6];
	CImage cartoon_bg;
	CImage main_screen;
	CImage start_button;
	CImage item_check;
	CImage click_cursor;
	CImage game_bg;
	CImage kirby_fly;
	CImage heart;
	CImage basic_bullet;
	CImage game_bg2;
	CImage skill;
	CImage skill_color;
	CImage skill_bullet;
	CImage m[5];
	CImage monster_bullet;
	CImage number;
	CImage hp_bar, hp_empty;
	CImage skill_effects;
	CImage game_over;
	CImage over_number;
	CImage kirby_hit;
	CImage kirby_zombie;
	CImage warning;
	CImage missile_explosion;
	CImage missile_bmp;
	CImage coin_bmp;
	CImage monster_create;
	CImage monster_die;
	CImage hit_effect;
	CImage rezero_bg;
	CImage kirby_die;
	CImage item[4];

	//sound resource
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


	ResourceManager(HINSTANCE g_hInst);


};

