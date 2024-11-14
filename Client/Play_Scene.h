#pragma once
#include "Scene.h"
#include "Player.h"
class Play_Scene :
    public Scene
{

private :
    //더블버퍼링
    HBITMAP hBitmap;
private:
    Player* player;

private:
    //ui image
    CImage* game_bg;
    CImage* game_bg2;
    CImage* number;
private :
    //play inform
    int bg_xPos = 800; // 배경 사진 xPos
    TCHAR number_text[99] = { '\0' };//점수, 골드 출력용
    int heart_cnt = 3; // 하트 갯수
    int play_score = 0;//이번판의 점수
    int play_gold = 0;//이번판의 돈

public:

    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock) {
        m_hwnd = hwnd;
        m_hBufferBitmap = hBufferBitmap;
        m_hBufferDC = hBufferDC;
        m_sock = sock;
        player = new Player();

        //ui 이미지 로드
        game_bg = &ResourceManager::getInstance().game_bg;
        game_bg2 = &ResourceManager::getInstance().game_bg2;
        number = &ResourceManager::getInstance().number;
    };

public:

    void render(LPVOID param) override;
    void ui_render();
    void player_render();
    void bullet_render();
    void enemy_render();
    void item_draw();
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};


