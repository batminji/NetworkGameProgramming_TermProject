#include "ResourceManager.h"

void ResourceManager::init(HINSTANCE g_hInst)
{
    
    //그래픽 리소스 로드
    Kirby_shield.LoadFromResource(g_hInst, MAKEINTRESOURCE(SHIELD));

    Kirby_pink_idle_left.LoadFromResource(g_hInst, MAKEINTRESOURCE(PINK_IDLE_LEFT));
    Kirby_blue_idle_left.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_IDLE_LEFT));
    Kirby_pink_idle_right.LoadFromResource(g_hInst, MAKEINTRESOURCE(PINK_IDLE_RIGHT));
    Kirby_blue_idle_right.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_IDLE_RIGHT));

    Room_screen.LoadFromResource(g_hInst, MAKEINTRESOURCE(ROOM_SCREEN));
    Dealer_check.LoadFromResource(g_hInst, MAKEINTRESOURCE(DEALER_CHECK));
    Healer_check.LoadFromResource(g_hInst, MAKEINTRESOURCE(HEALER_CHECK));
    Room_screen_bg.LoadFromResource(g_hInst, MAKEINTRESOURCE(ROOM_SCREEN_BG));

    Kirby_blue_fly.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_FLY_LEFT));
    Kirby_blue_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_HIT));
    //Kirby_blue_die.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_DIE));
    Kirby_blue_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(BLUE_ZOMBIE));

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
    Kirby_pink_fly.LoadFromResource(g_hInst, MAKEINTRESOURCE(PINK_FLY_LEFT));
    heart.LoadFromResource(g_hInst, MAKEINTRESOURCE(HEART));
    Kirby_pink_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(BASIC_BULLET));
    Kirby_blue_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(BASIC_BULLET_2));
    game_bg2.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_BACKGROUND_2));
    item[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM1));
    item[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM2));
    item[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM3));
    item[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM4));
    skill.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL));
    skill_color.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_COLOR));
    Kirby_pink_skill_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_BULLET));
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
    Kirby_pink_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(PINK_HIT));
    Kirby_pink_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(PINK_ZOMBIE));
    warning.LoadFromResource(g_hInst, MAKEINTRESOURCE(WARNING));
    missile_explosion.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE_EXPLOSION));
    missile_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE));
    coin_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(COIN));
    monster_create.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_CREATE));
    hit_effect.LoadFromResource(g_hInst, MAKEINTRESOURCE(HIT_EFFECT));
    monster_die.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_DIE));
    dual_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(STAR));
    magnet_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(MAGNET));
    

    // 사운드 리소스 로드
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

void ResourceManager::destroy()
{
    //이미지 리소스 해제
    Room_screen.Destroy();
    Dealer_check.Destroy();
    Healer_check.Destroy();
    Room_screen_bg.Destroy();

    Kirby_blue_fly.Destroy();
    Kirby_blue_hit.Destroy();
    Kirby_blue_zombie.Destroy();
    Kirby_blue_die.Destroy();

    start_screen.Destroy();
    title.Destroy();
    for (int i = 0; i < 6; ++i) cartoon[i].Destroy();
    cartoon_bg.Destroy();
    main_screen.Destroy();
    start_button.Destroy();
    item_check.Destroy();
    click_cursor.Destroy();
    game_bg.Destroy();
    Kirby_pink_fly.Destroy();
    Kirby_pink_die.Destroy();
    Kirby_pink_hit.Destroy();
    Kirby_pink_zombie.Destroy();
    heart.Destroy();
    //basic_bullet.Destroy();
    game_bg2.Destroy();
    skill.Destroy();
    skill_color.Destroy();
    //skill_bullet.Destroy();
    m[5].Destroy();
    monster_bullet.Destroy();
    number.Destroy();
    hp_bar, hp_empty.Destroy();
    skill_effects.Destroy();
    game_over.Destroy();
    over_number.Destroy();
    warning.Destroy();
    missile_explosion.Destroy();
    missile_bmp.Destroy();
    coin_bmp.Destroy();
    monster_create.Destroy();
    monster_die.Destroy();
    hit_effect.Destroy();
    rezero_bg.Destroy();
    for (int i = 0; i < 4; ++i) item[i].Destroy();

    //소리리소스 해제
    ssystem->close();
    ssystem->release();
}
