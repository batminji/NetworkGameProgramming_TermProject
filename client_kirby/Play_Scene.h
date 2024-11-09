#pragma once
#include "Scene.h"
#include "Player.h"
class Play_Scene :
    public Scene
{
private:
    Player* player;
public:

    Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC) {
        m_hwnd = hwnd;
        m_hBufferBitmap = hBufferBitmap;
        m_hBufferDC = hBufferDC;

        player = new Player();
    };

public:

    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};


