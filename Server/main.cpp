#include "stdafx.h"
#include "protocol.h"

/*
��Ŷ �Լ��� �⺻�� 
bool send_player_move_packet(SOCKET& s, std::string& id)
*/


class Player
{
private:
    SOCKET s = -1;
    std::string id = "";
    bool inGame = false; // ���� �������� ���̵��ΰ�?
    unsigned short y = 0; // winsize/2�� �Ұ�
    unsigned int high_score = 0;
    unsigned int coin = 0; // �����÷ο� ���� int�� ����

    // ������ �������� �� 

public:

    Player() {}

    bool broadcast(char* res, size_t size)
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // ���� ó��
            int error = WSAGetLastError();
            SERVER_err_display("send failed");
            SERVER_err_display(error);  // ���� �ڵ� ���
            return false;
        }
        return true;
    }

    void init(std::string& str, unsigned int high_score, unsigned short gold)
    {
        this->high_score = high_score;
        this->coin = gold;
        id = str;
        s = -1; // ���� ���̴ϱ� ���� �ʱ�ȭ
    }

    // getter - setter �η�
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
    std::string dealer_id; // ���� �����ΰ�.
    bool isPlaying = false;
    unsigned int score;
    unsigned short heart; // ������ ��..?
 
    // ��ü�� //

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

enum TASK_TYPE { FIRE_PLAYER_BULLET, FIRE_ENEMY_BULLET, AI_MOVE }; // ���� ������?
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

// ���������� ������ : ��ü ������ �����
bool save_all_player_info()
{
    // ����� ���� ���� ���� (�⺻ ����)
    std::ofstream out{ "data.txt", std::ios::out | std::ios::trunc };

    if (!out.is_open()) { // ������ ����� ���ȴ��� Ȯ��
        std::cerr << "Failed to open data.txt for writing" << std::endl;
        return false;
    }

    for (auto& pair : players) { // players ���� ��� �׸� ��ȸ
        auto& player = pair.second;
        out << player.getID() << " "   // ID
            << player.getCoin() << " " // Coin
            << player.getHigh_score()   // High Score
            << std::endl;              // �� �÷��̾� ������ �� �ٷ� ����
    }

    return true; // ���� �ڵ����� ���� (RAII)
}

// �� ���� �߰��ϱ�
bool add_new_player(const std::string& id, unsigned int coin, unsigned int highScore)
{
    // �߰� ���� ���� ����
    std::ofstream out{ "data.txt", std::ios::out | std::ios::app };

    if (!out.is_open()) { // ������ ����� ���ȴ��� Ȯ��
        std::cerr << "Failed to open data.txt for appending" << std::endl;
        return false;
    }

    // �� ���� �����͸� �߰�
    out << id << " " << coin << " " << highScore << std::endl;

    return true; // ���� �ڵ����� ���� (RAII)
}

// ���� ���� �߰��ϱ�(���� ����ÿ� �ε�)
bool read_player_info() // ������ ó��
{
    std::ifstream in{ "data.txt" }; // data.txt ���� ����

    if (!in.is_open()) { // ������ ����� ���ȴ��� Ȯ��
        std::cerr << "Failed to open data.txt" << std::endl;
        return false;
    }

    std::string stmp;
    unsigned int ctmp;
    unsigned int hstmp;
    while (in >> stmp >> ctmp >> hstmp) { // ID, coin, highScore �б�
        players[stmp].init(stmp, hstmp, ctmp);
    }

    in.close(); // ���� �ݱ�
    return true;
}

// ��ŷ
bool send_top_high_scores(SOCKET& s) {
    SC_RANKING_PACKET res;
    res.size = sizeof(SC_RANKING_PACKET);
    res.type = SC_RANKING;

    std::vector<std::pair<std::string, unsigned int>> playerData;
    for (auto& p : players) {
        playerData.emplace_back(p.second.getID(), p.second.getHigh_score());
    }

    // ������ �������� �������� ����
    std::sort(playerData.begin(), playerData.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; // ���� ��
        });

    // �ϵ�... ����
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

    // ��Ŷ ����
    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(res), 0);
    if (ret == SOCKET_ERROR) {
        SERVER_err_display("send() failed");
        return false;
    }
    return true;
}

bool send_login_packet(SOCKET& s, CS_LOGIN_PACKET* p) // �α��� ��Ŷ ������ �Լ� ���� ��.
{
    SC_LOGIN_RESULT_PACKET res;
    res.size = sizeof(SC_LOGIN_RESULT_PACKET);
    res.type = SC_LOGIN_RESULT;

    int k = players.count(p->id);
    auto& pl = players[p->id];
    if (pl.isPlaying()) { // �÷��̾ ���� �������ΰ�?
        res.success = false;
        res.is_new = false;
    }
    else {
        res.success = true;
        if (k == 0) // ���� ���̵��� �÷��̾ �����ϴ°�?
        {
            res.is_new = true;
            std::cout << "���ο� ���� " << p->id << "����!" << std::endl;
            add_new_player(p->id, pl.getCoin(), pl.getHigh_score());
        }
        else {
            res.is_new = false;
            std::cout << "���� ���� " << p->id << "����!" << std::endl;
        }

        pl.setPlaying(true);
        res.coin = pl.getCoin();
        res.high_score = pl.getHigh_score();
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (ret == SOCKET_ERROR) { // ���� ó��
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // ���� �ڵ� ���
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
    if (roomInfo[id].getP1ID() == id) {// ���� p1�̱�
        res.this_y = roomInfo[id].getP1Y();
        res.other_y = roomInfo[id].getP2Y();
    }
    else {
        res.this_y = roomInfo[id].getP2Y();
        res.other_y = roomInfo[id].getP1Y();
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // ���� ó��
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // ���� �ڵ� ���
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
    if (ret == SOCKET_ERROR) { // ���� ó��
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // ���� �ڵ� ���
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
    switch (packet[2]) // ���� ��Ȯ�ϰ� ��� �������� ���� �ҵ�
    {
    case CS_JOIN_ROOM: 
    {
        CS_JOIN_ROOM_PACKET* p = reinterpret_cast<CS_JOIN_ROOM_PACKET*>(packet);
        // ���� roomInfo�� p.id�� �� room�� ������ ����
        // �ƴҶ� ���� ���� �ʱ�ȭ

        if (roomInfo.find(p->id) != roomInfo.end()) {  // roomID�� �̹� �����ϸ�
            roomInfo[id] = roomInfo[p->id];
            roomInfo[id].setP2(&players[id]);       // �÷��̾ �ش� �濡 �߰�
            std::cout << "Player " << id  << " joined existing room " << p->id << std::endl;
        }
        else { // roomID�� ���ٸ� ���ο� ���� �����Ͽ� �ʱ�ȭ
            Room newRoom;
            roomInfo[id] = newRoom; // roomInfo�� ���ο� �� �߰�
            roomInfo[id].setP1(&players[id]);
            std::cout << "Player " << id << " created and joined new room " << id << std::endl;
        }

        if (false == send_room_change_packet(id)) return false; // ���� ����.

        break;
    }
    case CS_MOVE: // �÷��̾� ĳ������ �̵� -> ���� ������ ������Ʈ�� ��ġ ���� ��ȯ.
    {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        players[id].setY(p->y);
        if (false == send_player_move_packet(s, id)) return false;

        break;
    }
    case CS_KEY_INPUT: // �÷��̾��� Ű �Է�(��ų ��)
    {
        CS_KEY_INPUT_PACKET* p = reinterpret_cast<CS_KEY_INPUT_PACKET*>(packet); // ��� �ٷ� send ���� ����?
        break;
    }
    case CS_ROOM_STATE: // �� ���� ����
    {
        CS_ROOM_STATE_PACKET* p = reinterpret_cast<CS_ROOM_STATE_PACKET*>(packet);
        auto t_room = roomInfo[id];
        if (p->isQuit) { // ������
            if (t_room.getP1ID() == id)
                t_room.setP1(&players[t_room.getP2ID()]); // 2�� 1�� �ٲٱ�.
            t_room.setP2(nullptr);
        }
 
        if (p->isPlaying) t_room.setisPlaying(true); 
        if (p->isDealer) t_room.setDealer(id);
        else t_room.setDealer(t_room.getP2ID());

        if (false == send_room_change_packet(id)) return false; // ���� ����.
        break;
    }
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
    }
    return true;
}

int client_thread(SOCKET s) // Ŭ���̾�Ʈ���� ��� ������
{
    char recv_buf[BUFSIZE];
    std::string pid;

    // �α��� ó�� �ܰ�
    {
        // 1. �α��� ��Ŷ �ޱ�
        ZeroMemory(recv_buf, sizeof(recv_buf));
        int ret = recv(s, recv_buf, sizeof(CS_LOGIN_PACKET), 0);
        if (ret == SOCKET_ERROR) { // ����ó��
            int error = WSAGetLastError();
            SERVER_err_display("recv() failed");
            SERVER_err_display(error);  // ���� �ڵ� ���
            closesocket(s);
            return -1;
        }
        else if (ret == 0) {
            std::cout << "Ŭ���̾�Ʈ�� ������ �����ϴ�." << std::endl;
            closesocket(s);
            return 0;
        }
        

        // 1-1. �α��� ��Ŷ ó��
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(recv_buf);
        client_info(s, p->id);
        if (send_login_packet(s, p)){
            pid = p->id;
            send_top_high_scores(s);
        }
        
        players[pid].setSocket(s);

        // 2. �� ���� send - recv 
        while (true) {
            // 2-1. ������ ���� �÷��̾� ���̵� recv�ϱ�.
            ZeroMemory(recv_buf, sizeof(recv_buf));
            int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), 0);
            if (ret == SOCKET_ERROR) { // ����ó��
                int error = WSAGetLastError();
                SERVER_err_display("recv() failed");
                SERVER_err_display(error);  // ���� �ڵ� ���
            }
            process_packet(recv_buf, s, pid);

            if (true == roomInfo[pid].getisPlaying()) break; // ���� ����
        }

        while (true) {
            // send ���� player state�� ����.. ����.. �ƹ�ư ������ ��
            send_player_move_packet(s, pid);


            // recv.
        }

    }
}

// timer thread: ai ����
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

void push_evt_queue(TASK_TYPE ev, int time, std::string& rid) // time: milisecond ����.
{
    // todo
    EVENT evt;
    evt.setup(ev, time, rid);
    evt_queue.push(evt);
}

// task ó���ϴ� ������ -> �� �� �ϳ������� ���ڴµ� ��� �ϸ� ������ �𸣰���.
void ai_thread()
{
    while (true) {
        EVENT ev;
        if (evt_queue.empty()) std::this_thread::yield();
        evt_queue.try_pop(ev);
        switch (ev.evt_type) {
        case FIRE_PLAYER_BULLET:
            // todo: ���� �÷��̾� ��ġ���� bullet ��������� ��

            push_evt_queue(FIRE_PLAYER_BULLET, 1000, ev.room_id); // 1�ʿ� �Ѱ��� �߻�
            break;

        case FIRE_ENEMY_BULLET:
            // todo: ���� �� ��ġ���� bullet ��������� ��

            break;

        case AI_MOVE:
            // todo: ai ���� ����� �ű��

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

    std::vector<std::thread> threads;  // std::thread�� ������ ����

    while (true) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "accept() failed" << std::endl;
            break;
        }

        // ������ ����...
        threads.emplace_back(client_thread, client_sock);
    }

    for (auto& th : threads) {
         th.join();
    }

    closesocket(listen_sock);
    WSACleanup();
}