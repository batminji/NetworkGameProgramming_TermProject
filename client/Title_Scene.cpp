#include "Title_Scene.h"

Title_Scene::Title_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;

    start_screen = &ResourceManager::getInstance().start_screen;
    title = &ResourceManager::getInstance().title;
}

void Title_Scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    start_screen->StretchBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, SRCCOPY);
    title->TransparentBlt(m_hBufferDC, 10 - title_x, 10 - title_y, title_x + 800, title_y + 600, 0, 0, 800, 600, RGB(255, 0, 255));
    SetBkMode(m_hBufferDC, TRANSPARENT);
    TextOut(m_hBufferDC, 285, 460, ID, lstrlen(ID));
    
}

void Title_Scene::update()
{
    if (title_direction) {
        title_x += 4; title_y += 3;
        if (title_x == 20)title_direction = FALSE;
    }
    else {
        title_x -= 4; title_y -= 3;
        if (title_x == 0)title_direction = TRUE;
    }
}

LRESULT Title_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    case WM_CHAR:
            if (wParam == VK_BACK) {
                if (count == 0);
                else {
                    ID[count - 1] = NULL;
                    count--;
                }
            }
            else if (wParam == VK_RETURN) {
               // channel->stop();
               // title_bgm->release();
               // ssystem->playSound(click_sound, 0, false, &channel);
               // ssystem->playSound(cartoon_bgm, 0, false, &channel);
              
                CS_LOGIN_PACKET loginPacket;
                loginPacket.size = sizeof(CS_LOGIN_PACKET);
                loginPacket.type = CS_LOGIN;

                WideCharToMultiByte(CP_ACP, 0, ID, -1, loginPacket.id, sizeof(loginPacket.id), NULL, NULL);

                if (send(*m_sock, reinterpret_cast<char*>(&loginPacket), sizeof(loginPacket), 0) == SOCKET_ERROR) {
                    std::cerr << "Send failed." << std::endl;
                    closesocket(*m_sock);
                    WSACleanup();
                    return -1;
                }

                // 6. 로그인 결과 받기
                char recvBuf[BUFSIZE];
                int recvLen = recv(*m_sock, recvBuf, sizeof(SC_LOGIN_RESULT_PACKET), 0);
                if (recvLen <= 0) {
                    std::cerr << "Receive failed or connection closed." << std::endl;
                }
                else {
                    SC_LOGIN_RESULT_PACKET* resPacket = reinterpret_cast<SC_LOGIN_RESULT_PACKET*>(recvBuf);
                    if (resPacket->success) {
                        std::cout << "Login successful!" << std::endl;
                    }
                    else {
                        std::cout << "Login failed." << std::endl;
                    }
                }
            }
            else if (count < 10) ID[count++] = wParam;
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
