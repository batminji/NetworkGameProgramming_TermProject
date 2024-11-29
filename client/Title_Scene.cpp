#include "Title_Scene.h"

Title_Scene::Title_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;

    start_screen = &ResourceManager::getInstance().start_screen;
    title = &ResourceManager::getInstance().title;

    /*result = System_Create(&ssystem);
    if (result != FMOD_OK)
        exit(0);
    ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ssystem->createSound("sound_file/title_bgm.OGG", FMOD_LOOP_NORMAL, 0, &title_bgm);
    ssystem->createSound("sound_file/click.OGG", FMOD_DEFAULT, 0, &click_sound);
    ssystem->playSound(title_bgm, 0, false, &channel);*/
}

void Title_Scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    
    if(start_screen != nullptr)
        start_screen->StretchBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, SRCCOPY);
    if(title != nullptr)
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

void Title_Scene::network()
{

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
                // ssystem->playSound(click_sound, 0, false, &channel);

                CS_LOGIN_PACKET loginPacket;
                loginPacket.size = sizeof(CS_LOGIN_PACKET);
                loginPacket.type = CS_LOGIN;

                WideCharToMultiByte(CP_ACP, 0, ID, -1, loginPacket.id, sizeof(loginPacket.id), NULL, NULL);

               if (send(*m_sock, reinterpret_cast<char*>(&loginPacket), sizeof(loginPacket), 0) == SOCKET_ERROR) {
                   std::cerr << "로그인 Send failed." << std::endl;
                   closesocket(*m_sock);
                   WSACleanup();
                   //return -1;
               }
               
               // 6. 로그인 결과 받기
               char recvBuf[BUFSIZE];
               ZeroMemory(recvBuf, sizeof(recvBuf));
               int recvLen = recv(*m_sock, recvBuf, sizeof(SC_LOGIN_RESULT_PACKET), 0);
               if (recvLen <= 0) {
                   std::cerr << "Receive failed or connection closed." << std::endl;
               }
               else {
                   SC_LOGIN_RESULT_PACKET* resPacket = reinterpret_cast<SC_LOGIN_RESULT_PACKET*>(recvBuf);
                   if (resPacket->success) {
                       std::cout << "Login successful!" << std::endl;
                       if (resPacket->is_new) {
                           std::cout << "새유저!" << std::endl;
                           next_scene = CARTOON_SCENE;
                           strcpy(DataManager::getInstance().my_data.ID, loginPacket.id);
                           DataManager::getInstance().my_data.high_score = 0;
                           DataManager::getInstance().my_data.coin = 0;
                           /*channel->stop();
                           title_bgm->release();
                           ssystem->close();
                           ssystem->release();*/
                       }
                       else {
                           std::cout << "기존유저!" << std::endl;
                           next_scene = LOBBY_SCENE;
                           strcpy(DataManager::getInstance().my_data.ID, loginPacket.id);
                           DataManager::getInstance().my_data.high_score = resPacket->high_score;
                           DataManager::getInstance().my_data.coin = resPacket->coin;
                           /*channel->stop();
                           title_bgm->release();
                           ssystem->close();
                           ssystem->release();*/
                       }
                   }
                   else {
                       std::cout << "Login failed." << std::endl;
                       MessageBox(
                           NULL,              
                           L"해당 아이디는 이미 접속중입니다",      
                           L"로그인 실패",           
                           MB_OK             
                       );
                       if (*m_sock != INVALID_SOCKET) {
                           shutdown(*m_sock, SD_BOTH); // 양방향 종료 알림
                           closesocket(*m_sock);      // 소켓 닫기
                       }

                       // Winsock 정리
                       WSACleanup();
                       PostQuitMessage(0);
                   }
               }
              
            }
            else if (count < 20) ID[count++] = wParam;

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
