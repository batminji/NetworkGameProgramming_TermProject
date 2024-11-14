#include "stdafx.h"
#include "protocol.h"

/*
패킷 함수의 기본형 
bool send_player_move_packet(SOCKET& s, std::string& id)
*/

class Player
{
private:
    int pid; // 서버에서만 사용함
    SOCKET s;
    std::string id;
    bool inGame = false; // 현재 접속중인 아이디인가?
    unsigned short y;
public:

    Player() {}

    bool broadcast(char* res, size_t size )
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // 에러 처리
            int error = WSAGetLastError();
            err_display("send failed");
            err_display(error);  // 오류 코드 출력
            return false;
        }
    }

    // getter - setter 부류
    bool isPlaying() { return inGame; }
    void setPlaying(bool x) { inGame = x; }
    unsigned short getY() { return y; }
    void setY(unsigned short y) { this->y = y; }
    std::string getID() { return id; }
    void setSocket(SOCKET& s) { this->s = s; }
};

class Room
{
private:
    Player* p1 = nullptr;
    Player* p2 = nullptr;
    std::string dealer_id; // 누가 딜러인가.
    bool isPlaying = false;
    unsigned int score;
    unsigned short heart; // 음수가 없..?
 
    // 객체들 //

public:
    Room() {};
    ~Room() {};
    
    // Getter - Setter
    unsigned short getP1Y() { return p1->getY(); }
    unsigned short getP2Y() { return p2->getY(); }
    std::string getP1ID() { return p1->getID(); }
    std::string getP2ID() { return p2->getID(); }
    void setP1(Player* p) { p1 = p; }
    void setP2(Player* p) { p2 = p; }
    void setisPlaying(bool a) { isPlaying = a; }
    bool getisPlaying() { return isPlaying; }
    bool getisDealer(std::string myid) { return myid == dealer_id; }
    bool broadcast(char* res, size_t size)
    {
        p1->broadcast(res, size);
        p2->broadcast(res, size);
    }
};

std::unordered_map<std::string, Room> roomInfo;
std::unordered_map<std::string, Player> players;

bool process_packet(char* packet, SOCKET& s, std::string& id)
{
    switch (packet[2]) // 여기 정확하게 어디 들어오는지 봐야 할듯
    {
    case CS_JOIN_ROOM: 
    {
        CS_JOIN_ROOM_PACKET* p = reinterpret_cast<CS_JOIN_ROOM_PACKET*>(packet);
        // 만약 roomInfo에 p.id로 된 room이 있을때 조인
        // 아닐때 새로 만들어서 초기화

        if (roomInfo.find(p->id) != roomInfo.end()) {  // roomID가 이미 존재하면
            roomInfo[id] = roomInfo[p->id];
            roomInfo[id].setP2(&players[id]);       // 플레이어를 해당 방에 추가
            std::cout << "Player " << id  << " joined existing room " << p->id << std::endl;
        }
        else { // roomID가 없다면 새로운 방을 생성하여 초기화
            Room newRoom;
            roomInfo[id] = newRoom; // roomInfo에 새로운 방 추가
            roomInfo[id].setP1(&players[id]);
            std::cout << "Player " << id << " created and joined new room " << id << std::endl;
        }

        break;
    }
    case CS_MOVE: // 플레이어 캐릭터의 이동 -> 나와 동료의 업데이트된 위치 정보 반환.
    {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        players[id].setY(p->y);
       // send_player_move_packet(s, id);
        break;
    }
    case CS_KEY_INPUT: // 플레이어의 키 입력(스킬 등)
    {
        CS_KEY_INPUT_PACKET* p = reinterpret_cast<CS_KEY_INPUT_PACKET*>(packet); // 얘는 바로 send 하지 말까?
        break;
    }
    case CS_ROOM_STATE: // 방 정보 변경
    {
        CS_ROOM_STATE_PACKET* p = reinterpret_cast<CS_ROOM_STATE_PACKET*>(packet);
        auto t_room = roomInfo[id];
        if (p->isQuit)
            if (t_room.getP1ID() == id) t_room.setP1(nullptr);
            else t_room.setP2(nullptr);
        if (p->isPlaying) t_room.setisPlaying(true);

      //  send_room_change_packet(id);
        break;
    }
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
    }
}

bool send_room_change_packet(std::string& id)
{
    auto t_room = roomInfo[id];
    SC_ROOM_CHANGE_PACKET res;
    res.size = sizeof(SC_ROOM_CHANGE_PACKET);
    res.type = SC_ROOM_CHANGE;
    res.isPlaying = t_room.getisPlaying();
    res.isDealer = t_room.getisDealer(id);
    if (t_room.getP1ID() == id)  memcpy(res.other_pl, t_room.getP2ID().c_str(), ID_LEN);
    else memcpy(res.other_pl, t_room.getP1ID().c_str(), ID_LEN);
   
    if (t_room.broadcast(reinterpret_cast<char*>(&res), res.size))
        return true;
    return false;
}

bool send_player_move_packet(SOCKET& s, std::string& id)
{
    SC_PLAYER_MOVE_PACKET res;
    res.size = sizeof(SC_PLAYER_MOVE_PACKET);
    res.type = SC_PLAYER_MOVE;
    res.this_y = roomInfo[id].getP1Y();
    res.other_y = roomInfo[id].getP2Y();
    
    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        err_display("send failed");
        err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}


bool send_login_packet(SOCKET& s, CS_LOGIN_PACKET* p) // 로그인 패킷 보내는 함수 따로 뺌.
{
    SC_LOGIN_RESULT_PACKET res;
    res.size = sizeof(SC_LOGIN_RESULT_PACKET);
    res.type = SC_LOGIN_RESULT;
    if (players[p->id].isPlaying()) { // 플레이어가 지금 접속중인가?
        res.success = false;
    }
    else {
        res.success = true;
        players[p->id].setPlaying(true);
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        err_display("send failed");
        err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

int client_thread(SOCKET s) // 클라이언트와의 통신 스레드
{
    char recv_buf[BUFSIZE];
    std::string pid;

    // 로그인 처리 단계
    {
        // 1. 로그인 패킷 받기
        ZeroMemory(recv_buf, sizeof(recv_buf));
        int ret = recv(s, recv_buf, sizeof(CS_LOGIN_PACKET), 0);
        if (ret == SOCKET_ERROR) { // 에러처리
            int error = WSAGetLastError();
            err_display("recv() failed");
            err_display(error);  // 오류 코드 출력
            closesocket(s);
            return -1;
        }

        // 1-1. 로그인 패킷 처리
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(recv_buf);
        client_info(s, p->id);
        if (send_login_packet(s, p))
            pid = p->id;
        
        players[pid].setSocket(s);

        // 2. 방 관련 send - recv 
        while (true) {
            // 2-1. 참여할 방의 플레이어 아이디 recv하기.
            ZeroMemory(recv_buf, sizeof(recv_buf));
            int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), 0);
            if (ret == SOCKET_ERROR) { // 에러처리
                int error = WSAGetLastError();
                err_display("recv() failed");
                err_display(error);  // 오류 코드 출력
            }

            //CS_JOIN_ROOM_PACKET;


        }

    }
}

int main()
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;

    std::vector<std::thread> threads;  // std::thread를 저장할 벡터

    while (true) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "accept() failed" << std::endl;
            break;
        }

        // 스레드 생성...
        threads.emplace_back(client_thread, client_sock);
    }

    for (auto& th : threads) {
         th.join();
    }

    closesocket(listen_sock);
    WSACleanup();
}