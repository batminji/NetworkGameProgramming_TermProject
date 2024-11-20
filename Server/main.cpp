#include "stdafx.h"
#include "protocol.h"

/*
패킷 함수의 기본형 
bool send_player_move_packet(SOCKET& s, std::string& id)
*/


class Player
{
private:
    SOCKET s = -1;
    std::string id = "";
    bool inGame = false; // 현재 접속중인 아이디인가?
    unsigned short y = 0; // winsize/2로 할것
    unsigned int high_score = 0;
    unsigned int coin = 0; // 오버플로우 떠서 int로 수정

    // 게임을 실행중일 때 

public:

    Player() {}

    bool broadcast(char* res, size_t size)
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // 에러 처리
            int error = WSAGetLastError();
            SERVER_err_display("send failed");
            SERVER_err_display(error);  // 오류 코드 출력
            return false;
        }
        return true;
    }

    void init(std::string& str, unsigned int high_score, unsigned short gold)
    {
        this->high_score = high_score;
        this->coin = gold;
        id = str;
        s = -1; // 접속 전이니까 대충 초기화
    }

    // getter - setter 부류
    bool isPlaying() { return inGame; }
    void setPlaying(bool x) { inGame = x; }
    unsigned short getY() { return y; }
    void setY(unsigned short y) { this->y = y; }
    std::string getID() { return id; }
    void setSocket(SOCKET& s) { this->s = s; }
    unsigned short getCoin() { return coin; }
    unsigned int getHigh_score() { return high_score; }
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
    std::string getP2ID() 
    { 
        if (p2 == nullptr) return "";
        return p2->getID(); 
    }
    void setP1(Player* p) { p1 = p; }
    void setP2(Player* p) { p2 = p; }
    void setisPlaying(bool a) { isPlaying = a; }
    bool getisPlaying() { return isPlaying; }
    bool getisDealer(std::string myid) { return myid == dealer_id; }
    void setDealer(std::string id) { dealer_id = id; }
    bool broadcast(char* res, size_t size)
    {
        bool ret = false;
        if (p1 != nullptr) ret = p1->broadcast(res, size);
        if (p2 != nullptr) ret = ret && p2->broadcast(res, size);
        return ret;
    }
};

enum TASK_TYPE { FIRE_PLAYER_BULLET, FIRE_ENEMY_BULLET, AI_MOVE }; // 뭐가 있을까?
class EVENT
{
public:
    TASK_TYPE evt_type;
    std::string room_id; 
    std::chrono::system_clock::time_point do_time;

    EVENT() {}

    void setup(TASK_TYPE evt, int s_time, std::string& rid) // time -> ms
    {
        evt_type = evt;
        room_id = rid;
        do_time = std::chrono::system_clock::now() + std::chrono::milliseconds(s_time);
    }

    std::chrono::system_clock::time_point& getTime() { return do_time; }

    bool operator<(const EVENT& other) const
    {
        return do_time > other.do_time;
    }
};

std::unordered_map<std::string, Room> roomInfo;
std::unordered_map<std::string, Player> players;
concurrency::concurrent_priority_queue<EVENT> evt_queue;
concurrency::concurrent_queue<EVENT> task_queue;

// 유저데이터 관리용 : 전체 데이터 덮어쓰기
bool save_all_player_info()
{
    // 덮어쓰기 모드로 파일 열기 (기본 설정)
    std::ofstream out{ "data.txt", std::ios::out | std::ios::trunc };

    if (!out.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt for writing" << std::endl;
        return false;
    }

    for (auto& pair : players) { // players 맵의 모든 항목 순회
        auto& player = pair.second;
        out << player.getID() << " "   // ID
            << player.getCoin() << " " // Coin
            << player.getHigh_score()   // High Score
            << std::endl;              // 각 플레이어 정보를 한 줄로 저장
    }

    return true; // 파일 자동으로 닫힘 (RAII)
}

// 새 유저 추가하기
bool add_new_player(const std::string& id, unsigned int coin, unsigned int highScore)
{
    // 추가 모드로 파일 열기
    std::ofstream out{ "data.txt", std::ios::out | std::ios::app };

    if (!out.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt for appending" << std::endl;
        return false;
    }

    // 새 유저 데이터를 추가
    out << id << " " << coin << " " << highScore << std::endl;

    return true; // 파일 자동으로 닫힘 (RAII)
}

// 기존 유저 추가하기(서버 실행시에 로드)
bool read_player_info() // 데이터 처리
{
    std::ifstream in{ "data.txt" }; // data.txt 파일 열기

    if (!in.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt" << std::endl;
        return false;
    }

    std::string stmp;
    unsigned int ctmp;
    unsigned int hstmp;
    while (in >> stmp >> ctmp >> hstmp) { // ID, coin, highScore 읽기
        players[stmp].init(stmp, hstmp, ctmp);
    }

    in.close(); // 파일 닫기
    return true;
}

// 랭킹
bool send_top_high_scores(SOCKET& s) {
    SC_RANKING_PACKET res;
    res.size = sizeof(SC_RANKING_PACKET);
    res.type = SC_RANKING;

    std::vector<std::pair<std::string, unsigned int>> playerData;
    for (auto& p : players) {
        playerData.emplace_back(p.second.getID(), p.second.getHigh_score());
    }

    // 점수를 기준으로 내림차순 정렬
    std::sort(playerData.begin(), playerData.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; // 점수 비교
        });

    // 하드... 복붙
    int cnt = 0;

    strcpy(res.id1, playerData[cnt].first.c_str());
    res.hs1 = playerData[cnt].second;
    cnt++;

    strcpy(res.id2, playerData[cnt].first.c_str());
    res.hs2 = playerData[cnt].second;
    cnt++;

    strcpy(res.id3, playerData[cnt].first.c_str());
    res.hs3 = playerData[cnt].second;
    cnt++;
    
    strcpy(res.id4, playerData[cnt].first.c_str());
    res.hs4 = playerData[cnt].second;
    cnt++;
    
    strcpy(res.id5, playerData[cnt].first.c_str());
    res.hs5 = playerData[cnt].second;
    cnt++;

    // 패킷 전송
    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(res), 0);
    if (ret == SOCKET_ERROR) {
        SERVER_err_display("send() failed");
        return false;
    }
    return true;
}

bool send_login_packet(SOCKET& s, CS_LOGIN_PACKET* p) // 로그인 패킷 보내는 함수 따로 뺌.
{
    SC_LOGIN_RESULT_PACKET res;
    res.size = sizeof(SC_LOGIN_RESULT_PACKET);
    res.type = SC_LOGIN_RESULT;

    int k = players.count(p->id);
    auto& pl = players[p->id];
    if (pl.isPlaying()) { // 플레이어가 지금 접속중인가?
        res.success = false;
        res.is_new = false;
    }
    else {
        res.success = true;
        if (k == 0) // 같은 아이디의 플레이어가 존재하는가?
        {
            res.is_new = true;
            std::cout << "새로운 유저 " << p->id << "등장!" << std::endl;
            add_new_player(p->id, pl.getCoin(), pl.getHigh_score());
        }
        else {
            res.is_new = false;
            std::cout << "기존 유저 " << p->id << "등장!" << std::endl;
        }

        pl.setPlaying(true);
        res.coin = pl.getCoin();
        res.high_score = pl.getHigh_score();
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
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
    if (roomInfo[id].getP1ID() == id) {// 내가 p1이군
        res.this_y = roomInfo[id].getP1Y();
        res.other_y = roomInfo[id].getP2Y();
    }
    else {
        res.this_y = roomInfo[id].getP2Y();
        res.other_y = roomInfo[id].getP1Y();
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_player_state_change_packet(SOCKET& s, std::string& id)
{
    SC_PLAYER_STATE_CHANGE_PACKET res;
    res.size = sizeof(SC_PLAYER_STATE_CHANGE_PACKET);
    res.type = SC_PLAYER_STATE_CHANGE;

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_object_move_packet()
{
    return true;
}

bool send_object_change_packet()
{
    return true;
}


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

        if (false == send_room_change_packet(id)) return false; // 전송 실패.

        break;
    }
    case CS_MOVE: // 플레이어 캐릭터의 이동 -> 나와 동료의 업데이트된 위치 정보 반환.
    {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        players[id].setY(p->y);
        if (false == send_player_move_packet(s, id)) return false;

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
        if (p->isQuit) { // 나가기
            if (t_room.getP1ID() == id)
                t_room.setP1(&players[t_room.getP2ID()]); // 2를 1로 바꾸기.
            t_room.setP2(nullptr);
        }
 
        if (p->isPlaying) t_room.setisPlaying(true); 
        if (p->isDealer) t_room.setDealer(id);
        else t_room.setDealer(t_room.getP2ID());

        if (false == send_room_change_packet(id)) return false; // 전송 실패.
        break;
    }
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
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
            SERVER_err_display("recv() failed");
            SERVER_err_display(error);  // 오류 코드 출력
            closesocket(s);
            return -1;
        }
        else if (ret == 0) {
            std::cout << "클라이언트가 접속을 종료하다." << std::endl;
            closesocket(s);
            return 0;
        }
        

        // 1-1. 로그인 패킷 처리
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(recv_buf);
        client_info(s, p->id);
        if (send_login_packet(s, p)){
            pid = p->id;
            send_top_high_scores(s);
        }
        
        players[pid].setSocket(s);

        // 2. 방 관련 send - recv 
        while (true) {
            // 2-1. 참여할 방의 플레이어 아이디 recv하기.
            ZeroMemory(recv_buf, sizeof(recv_buf));
            int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), 0);
            if (ret == SOCKET_ERROR) { // 에러처리
                int error = WSAGetLastError();
                SERVER_err_display("recv() failed");
                SERVER_err_display(error);  // 오류 코드 출력
            }
            process_packet(recv_buf, s, pid);

            if (true == roomInfo[pid].getisPlaying()) break; // 게임 시작
        }

        while (true) {
            // send 먼저 player state랑 뭐지.. 뭐지.. 아무튼 보내야 함
            send_player_move_packet(s, pid);


            // recv.
        }

    }
}

// timer thread: ai 동작
void timer_thread()
{
    while (true)
    {
        EVENT ev;
        bool event_processed = false;

        if (evt_queue.try_pop(ev))
        {
            if (ev.getTime() <= std::chrono::system_clock::now())
                task_queue.push(ev);
            else
                evt_queue.push(ev);
        }
    }
}

void push_evt_queue(TASK_TYPE ev, int time, std::string& rid) // time: milisecond 단위.
{
    // todo
    EVENT evt;
    evt.setup(ev, time, rid);
    evt_queue.push(evt);
}

// task 처리하는 스레드 -> 룸 당 하나였으면 좋겠는데 어떻게 하면 좋을지 모르겠음.
void ai_thread()
{
    while (true) {
        EVENT ev;
        if (evt_queue.empty()) std::this_thread::yield();
        evt_queue.try_pop(ev);
        switch (ev.evt_type) {
        case FIRE_PLAYER_BULLET:
            // todo: 여기 플레이어 위치에서 bullet 생성해줘야 함

            push_evt_queue(FIRE_PLAYER_BULLET, 1000, ev.room_id); // 1초에 한개씩 발사
            break;

        case FIRE_ENEMY_BULLET:
            // todo: 여기 적 위치에서 bullet 생성해줘야 함

            break;

        case AI_MOVE:
            // todo: ai 로직 여기로 옮기기

            break;

        default:
            std::cout << "unknown event" << std::endl;
            exit(-1);
        }
    }
}


int main()
{
    read_player_info();

    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) SERVER_err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) SERVER_err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) SERVER_err_quit("listen()");

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