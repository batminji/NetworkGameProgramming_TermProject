#include "stdafx.h"
#include "protocol.h"

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

    // getter - setter �η�
    bool isPlaying() { return inGame; }
    void setPlaying(bool x) { inGame = x; }
};

class Room
{
private:
    Player* p1 = nullptr;
    Player* p2 = nullptr;
    std::string host_id;
    bool isPlaying = false;
    unsigned int score;
    unsigned short heart; // ������ ��..?
 
    // ��ü�� //

public:
    Room() {};
    ~Room() {};
};

std::unordered_map<std::string, Room> roomInfo;
std::unordered_map<std::string, Player> players;

bool process_packet(char* packet)
{
    switch (packet[2]) // ���� ��Ȯ�ϰ� ��� �������� ���� �ҵ�
    {
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
    }
}

int client_thread(SOCKET s) // Ŭ���̾�Ʈ���� ��� ������
{
    char recv_buf[BUFSIZE];
    char send_buf[BUFSIZE];

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

        // ��Ŷ ���� ������
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
        ZeroMemory(send_buf, sizeof(send_buf));
        memcpy(send_buf, &res, sizeof(res));

        send(s, send_buf, sizeof(SC_LOGIN_RESULT_PACKET), 0);
    }
}

std::atomic_int playerID; // �÷��̾� ���̵�

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