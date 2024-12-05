#pragma once
#include "Scene.h"
#include "ResourceManager.h"
#include "DataManager.h"
//#include "../Server/protocol.h"
class Title_Scene :
    public Scene
{
private:
    CImage *start_screen;
    CImage *title;
private:
    int title_x =0, title_y=0;
    BOOL title_direction = TRUE;

    TCHAR ID[20] = { '\0' };
    short count = 0;

public:
    System* ssystem = nullptr;
    Sound* title_bgm = nullptr;
    Sound* click_sound = nullptr;
    Channel* channel = nullptr;
    FMOD_RESULT result{};
    void* extradriverdata = 0;


public:
    Title_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock);
    void render(LPVOID param) override;
    void update() override;
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    void rank_recv();
};

