#include "Room_Scene.h"
//#include "../Server/protocol.h"
Room_Scene::Room_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, bool is_master) {
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;
    if (is_master) { // 내가 방장일 경우
        master_player = new Player(1, "1", TRUE);
        join_player = new Player(2, "1", FALSE);
    }
    else { // 방에 참여하는 경우
        master_player = new Player(1, "1", FALSE);
        join_player = new Player(2, "1", TRUE);
    }

    ////player
    //master_player = new Player(1, "1");
    //join_player = new Player(2, "1");

    //ui 이미지 로드
    room_screen = &ResourceManager::getInstance().Room_screen;
    room_screen_bg = &ResourceManager::getInstance().Room_screen_bg;
    dealer_check = &ResourceManager::getInstance().Dealer_check;
    healer_check = &ResourceManager::getInstance().Healer_check;

}
void Room_Scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    room_screen_bg->StretchBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, SRCCOPY);
    room_screen->TransparentBlt(m_hBufferDC, -20, -20, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));

    // 플레이어 그리기

    // 역할 체크
    if (master_player->who_is_me) { // 내가 방장이면
        switch (master_player->job) {
        case 1:
            dealer_check->TransparentBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        case 2:
            healer_check->TransparentBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        }
    }
    else if (join_player->who_is_me) { // 내가 조인이면
        switch (join_player->job) {
        case 1:
            dealer_check->TransparentBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        case 2:
            healer_check->TransparentBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        }
    }
}

void Room_Scene::update()
{
}

LRESULT Room_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, 800, 600, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);
        POINT mypt = { mx,my };
        if (master_player->who_is_me) {
            if (PtInRect(&dealer_rt, mypt)) { // 딜러 누르기
                master_player->job = 1;
                join_player->job = 2;
            }
            else if (PtInRect(&healer_rt, mypt)) { // 딜러 누르기
                master_player->job = 2;
                join_player->job = 1;
            }
            else if (PtInRect(&start_rt, mypt)) { // 딜러 누르기
                next_scene = PLAY_SCENE;
            }
        }
    }
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        default:break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
