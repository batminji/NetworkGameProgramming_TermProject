#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "DataManager.h"

class Lobby_Scene :
    public Scene
{
public:
    CImage* main_screen;
    CImage* start_button;
public:
    BOOL stbt_direction = TRUE;
    RECT stbt_rt = { 460,440,760,560 };
    short stbt_x = 0; short stbt_y = 0;

public:
    Lobby_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock);
    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

