#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "DataManager.h"

class Lobby_Scene :
    public Scene
{
public:
    CImage* main_screen;
public:
    BOOL stbt_direction = TRUE;
    RECT stbt_rt = { 460,440,760,560 };
    RECT create_room_button = {455,380,780,480};
    RECT join_room_button = { 455,485,780,580 };
    short stbt_x = 0; short stbt_y = 0;
public:
    TCHAR join_room_id[20] = {'\0'};
    short input_cnt = 0;
public:
    Lobby_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock);
    void render(LPVOID param) override;
    void network() override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

