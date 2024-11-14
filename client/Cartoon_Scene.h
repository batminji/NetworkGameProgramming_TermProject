#pragma once
#include "Scene.h"
#include "ResourceManager.h"

class Cartoon_Scene :
    public Scene
{
public:
    short click_xPos = 0;
    short cartoon_cnt = 0;
public:
    CImage* cartoon_bg = &ResourceManager::getInstance().cartoon_bg;
    CImage* cartoon[6] ;
    CImage* click_cursor = &ResourceManager::getInstance().click_cursor;

public:
    Cartoon_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock);
    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

