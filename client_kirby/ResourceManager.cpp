#include "ResourceManager.h"

ResourceManager::ResourceManager(HINSTANCE g_hInst)
{
	//그래픽 리소스 로드

	title.LoadFromResource(g_hInst, MAKEINTRESOURCE(TITLE));
	start_screen.LoadFromResource(g_hInst, MAKEINTRESOURCE(LOGIN_SCREEN));
	cartoon[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_1));
	cartoon[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_2));
	cartoon[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_3));
	cartoon[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_4));
	cartoon[4].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_5));
	cartoon[5].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_6));
	cartoon_bg.LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_BG));

	main_screen.LoadFromResource(g_hInst, MAKEINTRESOURCE(MAIN_SCREEN));
	start_button.LoadFromResource(g_hInst, MAKEINTRESOURCE(START_BUTTON));
	item_check.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM_CHECK));
	click_cursor.LoadFromResource(g_hInst, MAKEINTRESOURCE(CLiCK_CURSOR));

	game_bg.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_BACKGROUND));
	kirby_fly.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_FLY_LEFT));
	heart.LoadFromResource(g_hInst, MAKEINTRESOURCE(HEART));
	basic_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(BASIC_BULLET));
	game_bg2.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_BACKGROUND_2));
	item[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM1));
	item[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM2));
	item[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM3));
	item[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM4));

	skill.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL));
	skill_color.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_COLOR));
	skill_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_BULLET));

	m[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(SMALL_MONSTER));
	m[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID1_MONSTER));
	m[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID2_MONSTER));
	m[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID3_MONSTER));
	m[4].LoadFromResource(g_hInst, MAKEINTRESOURCE(BIG_MONSTER));
	monster_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(ENEMY_BULLET));
	number.LoadFromResource(g_hInst, MAKEINTRESOURCE(NUMBER));
	hp_bar.LoadFromResource(g_hInst, MAKEINTRESOURCE(HP_BAR));
	hp_empty.LoadFromResource(g_hInst, MAKEINTRESOURCE(HP_EMPTY));
	skill_effects.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_EFFECTS));
	game_over.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_OVER));
	over_number.LoadFromResource(g_hInst, MAKEINTRESOURCE(OVER_NUMBER));

	kirby_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_HIT));
	kirby_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_ZOMBIE));

	warning.LoadFromResource(g_hInst, MAKEINTRESOURCE(WARNING));
	missile_explosion.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE_EXPLOSION));
	missile_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE));

	coin_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(COIN));
	monster_create.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_CREATE));
	hit_effect.LoadFromResource(g_hInst, MAKEINTRESOURCE(HIT_EFFECT));
	monster_die.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_DIE));


	//사운드리소스 로드
	result = System_Create(&ssystem);
	if (result != FMOD_OK)
		exit(0);
	ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	ssystem->createSound("title_bgm.OGG", FMOD_LOOP_NORMAL, 0, &title_bgm);
	ssystem->createSound("cartoon_bgm.OGG", FMOD_LOOP_NORMAL, 0, &cartoon_bgm);
	ssystem->createSound("click.OGG", FMOD_DEFAULT, 0, &click_sound);
	ssystem->createSound("damage.OGG", FMOD_DEFAULT, 0, &damage_sound);
	ssystem->createSound("gameover.OGG", FMOD_DEFAULT, 0, &gameover_sound);
	ssystem->createSound("Shop.OGG", FMOD_DEFAULT, 0, &shop_sound);
	ssystem->createSound("Skill.OGG", FMOD_DEFAULT, 0, &skill_sound);
	ssystem->createSound("buzzer.OGG", FMOD_DEFAULT, 0, &cant_shop_sound);
	ssystem->createSound("cancle.OGG", FMOD_DEFAULT, 0, &shop_cancle_sound);
	ssystem->createSound("Explosion.OGG", FMOD_DEFAULT, 0, &explosion_sound);
	ssystem->createSound("warning.OGG", FMOD_DEFAULT, 0, &warning_sound);
	ssystem->createSound("coin.OGG", FMOD_DEFAULT, 0, &coin_sound);


}
