#include "stdafx.h"
#include "protocol.h"

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

    // getter - setter 부류
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
    unsigned short heart; // 음수가 없..?
 
    // 객체들 //

public:
    Room() {};
    ~Room() {};
};

std::unordered_map<std::string, Room> roomInfo;
std::unordered_map<std::string, Player> players;

bool process_packet(char* packet)
{
    switch (packet[2]) // 여기 정확하게 어디 들어오는지 봐야 할듯
    {
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
    }
}

int client_thread(SOCKET s) // 클라이언트와의 통신 스레드
{
    char recv_buf[BUFSIZE];
    char send_buf[BUFSIZE];

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

        // 패킷 만들어서 보내기
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
        ZeroMemory(send_buf, sizeof(send_buf));
        memcpy(send_buf, &res, sizeof(res));

        send(s, send_buf, sizeof(SC_LOGIN_RESULT_PACKET), 0);
    }
}

std::atomic_int playerID; // 플래이어 아이디

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