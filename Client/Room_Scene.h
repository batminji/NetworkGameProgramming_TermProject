#pragma once
#include "Scene.h"
#include "Player.h"

class Room_Scene :
    public Scene
{
private:
    CImage* room_screen;
    CImage* dealer_check;
    CImage* healer_check;
    CImage* room_screen_bg;
    CImage* pink_idle_left;
    CImage* pink_idle_right;
    CImage* blue_idle_left;
    CImage* blue_idle_right;
    short frame;

private:
    Player* master_player;
    Player* join_player;

public:
    RECT dealer_rt = { 50,470,177,545 };
    RECT healer_rt = { 230,470,370,545 };
    RECT start_rt = { 580,470,700,545 };

    Player* getMasterPlayer() const { return master_player; }
    Player* getJoinPlayer() const { return join_player; }

public:
    Room_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, bool is_master);
    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

