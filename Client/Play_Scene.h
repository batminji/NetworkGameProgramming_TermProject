#pragma once
#include "Scene.h"
#include "Player.h"
class Play_Scene :
    public Scene
{

private :
    //������۸�
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
    int bg_xPos = 800; // ��� ���� xPos
    TCHAR number_text[99] = { '\0' };//����, ��� ��¿�
    int heart_cnt = 3; // ��Ʈ ����
    int play_score = 0;//�̹����� ����
    int play_gold = 0;//�̹����� ��

public:

    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock) {
        m_hwnd = hwnd;
        m_hBufferBitmap = hBufferBitmap;
        m_hBufferDC = hBufferDC;
        m_sock = sock;
        player = new Player();

        //ui �̹��� �ε�
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


