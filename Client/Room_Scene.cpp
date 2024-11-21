#include "Room_Scene.h"
//#include "../Server/protocol.h"
Room_Scene::Room_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock, bool is_master) {
    m_hwnd = hwnd;
    m_hBufferBitmap = hBufferBitmap;
    m_hBufferDC = hBufferDC;
    m_sock = sock;
    frame = 0;
    isPlaying = FALSE;
    if (is_master) { // 내가 방장일 경우
        master_player = new Player(1, TRUE);
        join_player = new Player(2, FALSE);
        join_player->room = FALSE; // 아직 상대가 들어오지 않음. 들어오면 TRUE
    }
    else { // 방에 참여하는 경우
        master_player = new Player(1, FALSE);
        join_player = new Player(2, TRUE);
        join_player->room = TRUE;
    }

    //ui 이미지 로드
    room_screen = &ResourceManager::getInstance().Room_screen;
    room_screen_bg = &ResourceManager::getInstance().Room_screen_bg;
    dealer_check = &ResourceManager::getInstance().Dealer_check;
    healer_check = &ResourceManager::getInstance().Healer_check;
    pink_idle_left = &ResourceManager::getInstance().Kirby_pink_idle_left;
    pink_idle_right = &ResourceManager::getInstance().Kirby_pink_idle_right;
    blue_idle_left = &ResourceManager::getInstance().Kirby_blue_idle_left;
    blue_idle_right = &ResourceManager::getInstance().Kirby_blue_idle_right;
}
void Room_Scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    room_screen_bg->StretchBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, SRCCOPY);
    room_screen->TransparentBlt(m_hBufferDC, -20, -20, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));

    // 플레이어 그리기

    //방장 이름 출력
    SetBkMode(m_hBufferDC, TRANSPARENT);
    TextOut(m_hBufferDC,127, 130, user_name[0], lstrlen(user_name[0]));

    switch (master_player->job) {
    case 1:
        pink_idle_right->TransparentBlt(m_hBufferDC, 100, 180, 200, 200, frame * 25, 0, 25, 25, RGB(0, 255, 0));
        break;
    case 2:
        blue_idle_right->TransparentBlt(m_hBufferDC, 100, 180, 200, 200, frame * 25, 0, 25, 25, RGB(0, 255, 0));
        break;
    }
    if (join_player->room) { // 조인이 들어왔으면..

        //팀원 이름 출력
        SetBkMode(m_hBufferDC, TRANSPARENT);
        TextOut(m_hBufferDC, 478, 130, user_name[1], lstrlen(user_name[1]));

        switch (join_player->job) {
        case 1:
            pink_idle_left->TransparentBlt(m_hBufferDC, 474, 180, 200, 200, frame * 25, 0, 25, 25, RGB(0, 255, 0));
            break;
        case 2:
            blue_idle_left->TransparentBlt(m_hBufferDC, 474, 180, 200, 200, frame * 25, 0, 25, 25, RGB(0, 255, 0));
            break;
        }
    }

    // 역할 체크
    if (master_player->who_is_me) { // 내가 방장이면
        switch (master_player->job) {
        case 1:
            dealer_check->TransparentBlt(m_hBufferDC, -10, -10, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        case 2:
            healer_check->TransparentBlt(m_hBufferDC, -10, -10, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        }
    }
    else if (join_player->who_is_me) { // 내가 조인이면
        switch (join_player->job) {
        case 1:
            dealer_check->TransparentBlt(m_hBufferDC, -10, -10, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        case 2:
            healer_check->TransparentBlt(m_hBufferDC, -10, -10, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
            break;
        }
    }
}

void Room_Scene::update()
{
    //애니메이션 프레임 갱신
    frame = (frame + 1) % 6;

    room_data_update();
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
        printf("x : %d y : %d\n", mx, my);
        if (master_player->who_is_me) {
            if (PtInRect(&dealer_rt, mypt)) { // 딜러 누르기
                master_player->job = 1;
                join_player->job = 2;
            }
            else if (PtInRect(&healer_rt, mypt)) { // 힐러 누르기
                master_player->job = 2;
                join_player->job = 1;
            }
            else if (PtInRect(&start_rt, mypt)) { //시작 누르기
                isPlaying = TRUE;
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

int Room_Scene::room_data_update()
{
    //송신
    if (join_player->who_is_me) { // 조인은 송신 금지띠
        CS_ROOM_STATE_PACKET roomPacket;
        roomPacket.size = sizeof(CS_ROOM_STATE_PACKET);
        roomPacket.type = CS_ROOM_STATE;
        (master_player->job == 1) ? roomPacket.isDealer = true : roomPacket.isDealer = false;
        if (isPlaying)roomPacket.isPlaying = TRUE;
        else roomPacket.isPlaying = FALSE;
        roomPacket.isQuit = false;

        if (send(*m_sock, reinterpret_cast<char*>(&roomPacket), sizeof(roomPacket), 0) == SOCKET_ERROR) {
            std::cerr << "방상태 전송실패" << std::endl;
            closesocket(*m_sock);
            WSACleanup();
        }
    }

    //수신

    char recvBuf[BUFSIZE];
    int recvLen = recv(*m_sock, recvBuf, sizeof(SC_ROOM_CHANGE_PACKET), 0);
    if (recvLen <= 0) {
        std::cerr << "Receive failed or connection closed." << std::endl;
    }
    else {
        SC_ROOM_CHANGE_PACKET* roomPacket = reinterpret_cast<SC_ROOM_CHANGE_PACKET*>(recvBuf);
       if (true == roomPacket->isPlaying) {
     
       }
       else {
  

            //TODO: 이제 이걸 잘써야함 넘졸령
            //roomPacket->other_pl[ID_LEN]; // 친구 플레이어 이름
            //roomPacket->isPlaying; // 시작했는지
            //roomPacket->isDealer; // 내가 딜러인가?
            if (master_player->who_is_me) {
                strcpy(DataManager::getInstance().my_data.otherID, roomPacket->other_pl);
                MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().my_data.ID, -1, user_name[0], sizeof(DataManager::getInstance().my_data.ID));
                MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().my_data.otherID, -1, user_name[1], sizeof(DataManager::getInstance().my_data.otherID));

               if(strlen(roomPacket->other_pl)>=1) join_player->room = TRUE;
            }
            else if (join_player->who_is_me) {
                strcpy(DataManager::getInstance().my_data.otherID, roomPacket->other_pl);

                MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().my_data.otherID, -1, user_name[0], sizeof(DataManager::getInstance().my_data.otherID));
                MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().my_data.ID, -1, user_name[1], sizeof(DataManager::getInstance().my_data.ID));

                if (roomPacket->isDealer) {
                    master_player->job = 2; join_player->job = 1;
                }
                else {
                    master_player->job = 1; join_player->job = 2;
                }
           }
        }
    }

    return 1;
}
