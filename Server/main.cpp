#include "stdafx.h"
#include "protocol.h"

/*
��Ŷ �Լ��� �⺻�� 
bool send_player_move_packet(SOCKET& s, std::string& id)
*/

class Player
{
private:
    int pid; // ���������� �����
    SOCKET s;
    std::string id;
    bool inGame = false; // ���� �������� ���̵��ΰ�?
    unsigned short y;
public:

    Player() {}

    bool broadcast(char* res, size_t size )
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // ���� ó��
            int error = WSAGetLastError();
            err_display("send failed");
            err_display(error);  // ���� �ڵ� ���
            return false;
        }
    }

    // getter - setter �η�
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

        break;
    }
    case CS_MOVE: // �÷��̾� ĳ������ �̵� -> ���� ������ ������Ʈ�� ��ġ ���� ��ȯ.
    {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        players[id].setY(p->y);
       // send_player_move_packet(s, id);
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
    if (ret == SOCKET_ERROR) { // ���� ó��
        int error = WSAGetLastError();
        err_display("send failed");
        err_display(error);  // ���� �ڵ� ���
        return false;
    }
    return true;
}


bool send_login_packet(SOCKET& s, CS_LOGIN_PACKET* p) // �α��� ��Ŷ ������ �Լ� ���� ��.
{
    SC_LOGIN_RESULT_PACKET res;
    res.size = sizeof(SC_LOGIN_RESULT_PACKET);
    res.type = SC_LOGIN_RESULT;
    if (players[p->id].isPlaying()) { // �÷��̾ ���� �������ΰ�?
        res.success = false;
    }
    else {
        res.success = true;
        players[p->id].setPlaying(true);
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (ret == SOCKET_ERROR) { // ���� ó��
        int error = WSAGetLastError();
        err_display("send failed");
        err_display(error);  // ���� �ڵ� ���
        return false;
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
            err_display("recv() failed");
            err_display(error);  // ���� �ڵ� ���
            closesocket(s);
            return -1;
        }

        // 1-1. �α��� ��Ŷ ó��
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(recv_buf);
        client_info(s, p->id);
        if (send_login_packet(s, p))
            pid = p->id;
        
        players[pid].setSocket(s);

        // 2. �� ���� send - recv 
        while (true) {
            // 2-1. ������ ���� �÷��̾� ���̵� recv�ϱ�.
            ZeroMemory(recv_buf, sizeof(recv_buf));
            int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), 0);
            if (ret == SOCKET_ERROR) { // ����ó��
                int error = WSAGetLastError();
                err_display("recv() failed");
                err_display(error);  // ���� �ڵ� ���
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