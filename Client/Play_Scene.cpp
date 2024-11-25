﻿#include "Play_Scene.h"

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
    
    //총알 
    for (bullet b : bullets) b.render(m_hBufferDC);
    
    
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

void Play_Scene::bullet_draw()
{

    //나중에 그리다 터질때 이함수를 쓰도록하겟슴.
    
}

void Play_Scene::update()
{
    master_player->update();
    join_player->update();
    for (Enemy& e : enemys) e.update();
    for (bullet& b : bullets)b.update();

    //배경
    if (bg_xPos == 0) bg_xPos = 1600;
    else bg_xPos--;



}

void Play_Scene::network()
{
    // 플레이어 데이터 수신
    recv_player_data();
    //오브젝트 데이터 수신
    //recv_object_data();
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

int Play_Scene::recv_player_data() {
    // 수신 버퍼 및 누적 데이터 버퍼
    static std::vector<uint8_t> recvBuffer;
    char recvBuf[BUFSIZE];
    ZeroMemory(recvBuf, sizeof(recvBuf));

    // 데이터 수신
    int recvLen = recv(*m_sock, recvBuf, sizeof(recvBuf), 0); // MSG_WAITALL 대신 0 사용
    if (recvLen <= 0) {
        std::cerr << "플레이어 좌표받기 실패" << std::endl;
        return -1;
    }

    // 수신한 데이터를 누적 버퍼에 추가
    recvBuffer.insert(recvBuffer.end(), recvBuf, recvBuf + recvLen);

    // 패킷 처리
    while (recvBuffer.size() >= sizeof(SC_PLAYER_MOVE_PACKET)) { // 패킷 크기만큼 데이터가 도착했는지 확인
        SC_PLAYER_MOVE_PACKET* resPacket = reinterpret_cast<SC_PLAYER_MOVE_PACKET*>(recvBuffer.data());

        // 패킷 유효성 확인 (예: 패킷의 크기와 타입)
        if (recvBuffer.size() < resPacket->size) {
            break; // 전체 패킷이 아직 도착하지 않은 경우 대기
        }

        // 데이터 처리

        cout << "현재 실행중인 함수 : recv_player(7)    받은 패킷타입 :" << resPacket->type <<endl;

        if (master_player->who_is_me) {
            master_player->y = resPacket->this_y;
            join_player->y = resPacket->other_y;
           
        }
        else {
            join_player->y = resPacket->this_y;
            master_player->y = resPacket->other_y;
          
        }

        // 처리한 패킷 데이터를 누적 버퍼에서 제거
        recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + resPacket->size);
    }

    return 1;
}

std::mutex bullets_mutex;
int Play_Scene::recv_object_data() {
    // 데이터 저장용 임시 벡터
    vector<Enemy> temp_enemys;
    vector<bullet> temp_bullets;
    vector<Item> temp_items;
    temp_enemys.clear();
    temp_bullets.clear();
    temp_items.clear();

    static std::vector<uint8_t> recvBuffer;
    char recvBuf[BUFSIZE];
    ZeroMemory(recvBuf, sizeof(recvBuf));

    int recvLen = recv(*m_sock, recvBuf, sizeof(recvBuf), 0); // MSG_WAITALL ��� 0 ���
    if (recvLen <= 0) {
        std::cerr << "오브젝트좌표받기 실패" << std::endl;
        //  return -1;
    }

    recvBuffer.insert(recvBuffer.end(), recvBuf, recvBuf + recvLen);

   
    while (recvBuffer.size() >= sizeof(SC_OBJECT_MOVE_PACKET)) { 
        SC_OBJECT_MOVE_PACKET* resPacket = reinterpret_cast<SC_OBJECT_MOVE_PACKET*>(recvBuffer.data());

  
        if (recvBuffer.size() < resPacket->size) {
            break;
        }

        cout << "현재 실행중인 함수 : recv_object(9)    받은 패킷타입 :" << resPacket->type <<endl;
        for (int i = 0; i < resPacket->number; ++i) {
            switch (resPacket->objs_type[i]) {
            case ENEMY:
            
                break;
            case ENEMY_BULLETS: {
                bullet temp_bullet(resPacket->objs_x[i], resPacket->objs_y[i], 0, 3);
                temp_bullets.push_back(temp_bullet);
                break;
            }
            case MISSAIL:
     
                break;
            case P1_BULLET: {
                bullet temp_bullet(resPacket->objs_x[i], resPacket->objs_y[i], 1, 1);
                temp_bullets.push_back(temp_bullet);
                break;
            }
            case P2_BULLET: {
                bullet temp_bullet(resPacket->objs_x[i], resPacket->objs_y[i], 0, 2);
                temp_bullets.push_back(temp_bullet);
                break;
            }
            case P1_SKILLBULLET: {
                bullet temp_bullet(resPacket->objs_x[i], resPacket->objs_y[i], 2, 1);
                temp_bullets.push_back(temp_bullet);
                break;
            }
            case P2_SKILLBULLET:
         
                break;
            case ITEM:
         
                break;
            default:
                break;
            }
        }


        recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + resPacket->size);
    }

  
    {
        std::unique_lock<std::mutex> lock(bullets_mutex);
        bullets.insert(bullets.end(), std::make_move_iterator(temp_bullets.begin()), std::make_move_iterator(temp_bullets.end()));
        lock.unlock(); 
    }
}
