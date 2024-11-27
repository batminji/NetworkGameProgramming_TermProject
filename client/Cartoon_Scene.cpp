#include "Cartoon_Scene.h"

Cartoon_Scene::Cartoon_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;

    for (int i = 0; i < 6; i++) {
        cartoon[i] = &ResourceManager::getInstance().cartoon[i];
    }
    result = System_Create(&ssystem);
    if (result != FMOD_OK)
        exit(0);
    ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ssystem->createSound("sound_file/cartoon_bgm.OGG", FMOD_LOOP_NORMAL, 0, &cartoon_bgm);
    ssystem->createSound("sound_file/click.OGG", FMOD_DEFAULT, 0, &click_sound);
    ssystem->playSound(cartoon_bgm, 0, false, &channel);
}

void Cartoon_Scene::render(LPVOID param)
{
    cartoon_bg->StretchBlt(m_hBufferDC, 0, 0,800,600, 0, 0, 800, 600, SRCCOPY);
    for (int i = 0; i < cartoon_cnt + 1; ++i)
        cartoon[i]->TransparentBlt(m_hBufferDC, 0, 0,800,600, 0, 0, 800, 600, RGB(255, 0, 255));
    click_cursor->TransparentBlt(m_hBufferDC, 800 - 100,600 - 100, 90, 90, click_xPos, 0, 52, 52, RGB(255, 0, 255));
}

void Cartoon_Scene::network()
{
}

void Cartoon_Scene::update()
{
    click_xPos = (click_xPos + 52) % 104;
}

LRESULT Cartoon_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        cartoon_cnt++;
        ssystem->playSound(click_sound, 0, false, &channel);
        if (cartoon_cnt == 6) {
            next_scene = LOBBY_SCENE;
            channel->stop();
            cartoon_bgm->release();
            click_sound->release();
            ssystem->close();
            ssystem->release();
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
