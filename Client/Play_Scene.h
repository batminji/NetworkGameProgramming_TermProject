#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
//#include "../Server/protocol.h"

class Play_Scene :
    public Scene
{

private:
    Player* master_player;
    Player* join_player;

    vector<Enemy> enemys;
    vector<Item> Items;
private:
    //ui image
    CImage* game_bg;
    CImage* game_bg2;
    CImage* number;
    CImage* heart;
private :
    //play inform
    int bg_xPos = 800; // 배경 사진 xPos
    TCHAR number_text[99] = { '\0' };//점수, 골드 출력용

    POINT heart_pt[4] = { {765,25},{730,25},{695,25},{660,25} }; // 하트 찍는 위치
    int heart_cnt = 3; // 하트 갯수

    int play_score = 0;//이번판의 점수
    int play_gold = 0;//이번판의 돈

    //스킬키 눌럿더염?
    bool skill_key_down = false;

public:

    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, Player* p1, Player* p2);
    ~Play_Scene();
public:

    void render(LPVOID param) override;
    void ui_render();
    void enemy_render();
    void item_draw();
    void update() override;
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;


public:
    short send_y = 0;
    int send_player_input(short y);
   int recv_player_data();

   

};