#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "Bullet.h"
#include "DataManager.h"
//#include "../Server/protocol.h"

class Play_Scene :
    public Scene
{

private:
    Player* master_player;
    Player* join_player;

    vector<Enemy> enemys;
    vector<Item> Items;
    vector<bullet> bullets;
private:
    //ui image
    CImage* game_bg;
    CImage* game_bg2;
    CImage* number;
    CImage* heart;
    CImage* kirby_shield;
    CImage* skill_black;
    CImage* skill_color;
    CImage* game_over;
    CImage* over_number;
public:
    System* ssystem;
    Sound* battle_bgm;
    Sound* damage_sound;
    Sound* gameover_sound;
    Sound* skill_sound;
    Sound* explosion_sound;
    Sound* warning_sound;
    Sound* coin_sound;
    Channel* channel = 0;
    FMOD_RESULT result;
    void* extradriverdata = 0;
private :
    //play inform
    int bg_xPos = 800; // ��� ���� xPos
    TCHAR number_text[99] = { '\0' };//����, ��� ��¿�

    POINT heart_pt[4] = { {765,25},{730,25},{695,25},{660,25} }; // ��Ʈ ��� ��ġ
    unsigned short heart_cnt = 3; // ��Ʈ ����

    unsigned int play_score = 0;//�̹����� ����
    int play_gold = 0;//�̹����� ��

    //��ųŰ ��������?
    bool skill_key_down = false;

    
public:

    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, Player* p1, Player* p2);
    ~Play_Scene();
public:

    void render(LPVOID param) override;
    void ui_render();
    void enemy_render();
    void item_draw();
    void GameOver_draw();
    void update() override;
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;


public:
    short send_y = 300;
    int send_player_input(unsigned short y);
    // ������Ʈ ��ǥ ���� �Լ�
    int recv_process();
    void handle_player_data(const uint8_t* packetData);
    void handle_object_data(const uint8_t* packetData);

};