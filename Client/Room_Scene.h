#pragma once
#include "Scene.h"
#include "Player.h"
#include "DataManager.h"

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
public:
    System* ssystem = nullptr;
    Sound* room_bgm = nullptr;
    Sound* click_sound = nullptr;
    Channel* channel = 0;
    FMOD_RESULT result;
    void* extradriverdata = 0;

public:
    Player* master_player;
    Player* join_player;

public:
    WCHAR user_name[2][20]; //출력용 아이디, [0]이 방장. [1]이 팀원

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
    void network() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    std::atomic_bool isPlaying = false;

public:
    int room_data_update(); 
};

