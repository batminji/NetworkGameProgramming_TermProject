#pragma once
#include "Scene.h"
class test_scene :
    public Scene
{
public:
 
    bool isRunning = true;

    // 캐릭터 위치
    int characterX = 100;
    int characterY = 100;
    const int moveStep = 5; // 이동 속도

public :

    test_scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC):Scene(hwnd,hBufferBitmap,hBufferDC) {
       
    };
public:

    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};