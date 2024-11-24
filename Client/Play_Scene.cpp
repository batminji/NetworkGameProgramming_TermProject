#include "Play_Scene.h"

Play_Scene::Play_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, Player* p1, Player* p2) {
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;

    //player
    master_player = p1; join_player = p2;

    //ui 이미지 로드
    game_bg = &ResourceManager::getInstance().game_bg;
    game_bg2 = &ResourceManager::getInstance().game_bg2;
    number = &ResourceManager::getInstance().number;
    heart = &ResourceManager::getInstance().heart;
};

Play_Scene::~Play_Scene()
{
    if (master_player) delete master_player;
    if (join_player) delete join_player;

}

void Play_Scene::render(LPVOID param)
{
	m_hwnd = (HWND)param;
	RECT rect;
	GetClientRect(m_hwnd, &rect);

    //ui
    ui_render();
    // 캐릭터 그리기 (단순한 사각형으로 표현)
    master_player->render(m_hBufferDC);
    join_player->render(m_hBufferDC);
   
}

void Play_Scene::ui_render()
{
    game_bg->StretchBlt(m_hBufferDC, 0, 50, 800, 600 - 50, bg_xPos, 0, 800, 500, SRCCOPY);
    if (bg_xPos >= 800)game_bg->StretchBlt(m_hBufferDC, 1600 - bg_xPos, 50, bg_xPos - 800, 600 - 50, 0, 0, bg_xPos - 800, 500, SRCCOPY);
    game_bg2->StretchBlt(m_hBufferDC, 0, 0, 800, 50, 0, 0, 800, 50, SRCCOPY);
  //  // 아이템
  //  for (int i = 0; i < 4; ++i)
  //      if (item[i].buy)item[i].img.TransparentBlt(m_hBufferDC, 0, 0, 800, 50, 0, 0, 800, 50, RGB(255, 0, 255));
  //  // 하트
    for (int i = 0; i < heart_cnt; ++i)
        heart->TransparentBlt(m_hBufferDC, heart_pt[i].x - 15, heart_pt[i].y - 15, 30, 30, 0, 0, 18, 16, RGB(0, 255, 0));
   
  //  //점수
    int num;
    wsprintf(number_text, L"%d", play_score);
    if (lstrlen(number_text) == 0)number->TransparentBlt(m_hBufferDC, 10, 12, 24, 28, 0, 0, 12, 14, RGB(255, 0, 255));
    for (int i = 0; i < lstrlen(number_text); i++) {
        num = number_text[i] - 48;
        number->TransparentBlt(m_hBufferDC, i * 24 + 10, 12, 24, 28, num * 12, 0, 12, 14, RGB(255, 0, 255));
    }
   // 코인 수
    wsprintf(number_text, L"%d", play_gold);
    if (lstrlen(number_text) == 0)number->TransparentBlt(m_hBufferDC, 10, 12, 24, 28, 0, 0, 12, 14, RGB(255, 0, 255));
    for (int i = 0; i < lstrlen(number_text); i++) {
        num = number_text[i] - 48;
        number->TransparentBlt(m_hBufferDC, i * 24 + 520, 12, 24, 28, num * 12, 0, 12, 14, RGB(255, 0, 255));
    }
}

void Play_Scene::enemy_render()
{
    for (Enemy& e : enemys) e.render(m_hBufferDC);
}

void Play_Scene::item_draw()
{
    for (Item& i : Items) i.render(m_hBufferDC);
}

void Play_Scene::update()
{
    master_player->update();
    join_player->update();
    for (Enemy& e : enemys) e.update();

    //배경
    if (bg_xPos == 0) bg_xPos = 1600;
    else bg_xPos--;

}

void Play_Scene::network()
{
    // 플레이어 데이터 수신
    recv_player_data();
    // 플레이어인풋 전송
    send_player_input(send_y);
}

LRESULT Play_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    {
        if (wParam == VK_RETURN) { // 스킬을 썼다.
            // TODO 스킬 썼다는 패킷 보내야 함.
            skill_key_down = true;
            // 총알의 타입 변경 흠... 근데 흠...
        }
    }
        break;
    case WM_KEYUP:
        if (wParam == VK_RETURN) {
            skill_key_down = false;
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
    case WM_MOUSEMOVE:
        send_y = HIWORD(lParam);
         return 0;
    }
    

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int Play_Scene::send_player_input(unsigned short y)
{

    CS_MOVE_PACKET movePacket;
    movePacket.size = sizeof(CS_MOVE_PACKET);
    movePacket.type = CS_MOVE;
    movePacket.y = y;
    movePacket.keyDown = skill_key_down;

    if (send(*m_sock, reinterpret_cast<char*>(&movePacket), sizeof(movePacket), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed." << std::endl;
        closesocket(*m_sock);
        WSACleanup();
        return -1;
    }

    
    return 1;
}

int Play_Scene::recv_player_data()
{
    //플레이어 좌표 받기
    char recvBuf[BUFSIZE];
    ZeroMemory(recvBuf, sizeof(recvBuf));
    int recvLen = recv(*m_sock, recvBuf, sizeof(SC_PLAYER_MOVE_PACKET), MSG_WAITALL);
    if (recvLen <= 0) {
        std::cerr << "플레이어 좌표받기 실패" << std::endl;
        return -1;
    }
    else {
        SC_PLAYER_MOVE_PACKET* resPacket = reinterpret_cast<SC_PLAYER_MOVE_PACKET*>(recvBuf);
        if (master_player->who_is_me) {
            master_player->y = resPacket->this_y;
            join_player->y = resPacket->other_y;
            std::cout << "1P: " << resPacket->this_y << "  2P:" << resPacket->other_y << std::endl;
        }
        else {
            join_player->y = resPacket->this_y; // 1P의 좌표 값이 계속 돌어옴
            master_player->y = resPacket->other_y; // 계속 이상한 값이 들어옴

            // recvLen은 10으로 동일했음...
            // 패딩 오류도 아님...
            // 소켓 주소 때문인걸까?
            // 안되면 노트북 두 대로 각각 접속해서 확인해야 할 듯
            // 그것도 안되면 개망함 ㅋㅋ 모르겠음

            // 서버에서 room에 좌표는 잘 저장되어 있음.
            // 클라가 이상하게 읽는다기에는 1P가 잘 읽음.
            // 서버가 이상하게 보낸다기에는 2P 좌표는 잘 출력됨...
            
            // 서버에서 this_y other_y 구별하지 말고 그냥 보내도 2P한테는 이상하게 옴 ㅋㅋ
            std::cout << "2P:" << resPacket->this_y << "  1P:" << resPacket->other_y << std::endl;
            //std::cout << resPacket->other_y << " " << resPacket->size << " " << resPacket->this_y << " " << resPacket->type << std::endl;
        }
    }

    return 1;
}

