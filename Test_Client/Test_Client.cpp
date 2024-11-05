﻿#include "../Server/stdafx.h"
#include "../Server/protocol.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 9000

int main()
{
    const wchar_t* SERVER_IP = L"127.0.0.1";
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN serverAddr;

    // 1. 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    // 2. 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return -1;
    }

    // 3. 서버 주소 설정
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    // InetPton으로 IP 주소 설정
    if (InetPton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 4. 서버에 연결
    if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to server." << std::endl;

    // 5. 로그인 패킷 보내기
    CS_LOGIN_PACKET loginPacket;
    loginPacket.size = sizeof(CS_LOGIN_PACKET);
    loginPacket.type = CS_LOGIN;
    std::string s;
    std::cin >> s;

    strcpy_s(loginPacket.id, s.c_str()); // 예제 ID

    if (send(sock, reinterpret_cast<char*>(&loginPacket), sizeof(loginPacket), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 6. 로그인 결과 받기
    char recvBuf[BUFSIZE];
    int recvLen = recv(sock, recvBuf, sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (recvLen <= 0) {
        std::cerr << "Receive failed or connection closed." << std::endl;
    }
    else {
        SC_LOGIN_RESULT_PACKET* resPacket = reinterpret_cast<SC_LOGIN_RESULT_PACKET*>(recvBuf);
        if (resPacket->success) {
            std::cout << "Login successful!" << std::endl;
        }
        else {
            std::cout << "Login failed." << std::endl;
        }
    }

    // 7. 소켓 종료
    closesocket(sock);
    WSACleanup();

    return 0;
}