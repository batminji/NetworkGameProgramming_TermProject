#pragma once
#include "stdafx.h"
#include "Framework.h"
#include "ResourceManager.h"

//#include "../Server/protocol.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 9000
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI client_render(LPVOID param);
DWORD WINAPI client_update(LPVOID param);

HBITMAP hBufferBitmap;
HDC hBufferDC;
bool isRunning = true;

Framework* m_framework;


//네트워크
const wchar_t* SERVER_IP = L"127.0.0.1";
WSADATA wsaData;
SOCKET sock;
SOCKADDR_IN serverAddr;


void CreateConsole()
{ // 새로운 콘솔 창을 할당 
    AllocConsole(); // 콘솔 입출력을 표준 스트림에 연결 
    FILE* fp; freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONERR$", "w", stderr);
    std::cout << "콘솔 창이 생성되었습니다." << std::endl;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    CreateConsole(); // 메세지확인용 콘솔 생성
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

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    ResourceManager& resourceManager = ResourceManager::getInstance();
    resourceManager.init(hInstance); // hInstance를 전달하여 초기화

   

   
    HWND hwnd = CreateWindowEx(
        0,                    // dwExStyle: 확장 창 스타일 (없으면 0)
        CLASS_NAME,            // 클래스 이름
        L"Kirby_don't_overeating",       // 윈도우 이름
        WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;
  

    ShowWindow(hwnd, nCmdShow);
    HANDLE Render_Thread = NULL;
    HANDLE Update_Thread = NULL;
    // 렌더링 스레드 시작
    if (m_framework) {
       Render_Thread = CreateThread(NULL, 0, client_render, hwnd, 0, NULL);
    }
    // 업데이트 스레드 시작
    if (m_framework) {
        Update_Thread = CreateThread(NULL, 0, client_update, hwnd, 0, NULL);
    }
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 스레드 종료 설정
    isRunning = false;
    WaitForSingleObject(Render_Thread, INFINITE); // 스레드가 종료될 때까지 대기
    CloseHandle(Render_Thread); // 스레드 핸들 닫기
    WaitForSingleObject(Update_Thread, INFINITE); // 스레드가 종료될 때까지 대기
    CloseHandle(Update_Thread); // 스레드 핸들 닫기

    DeleteDC(hBufferDC);
    DeleteObject(hBufferBitmap);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
    if (uMsg == WM_CREATE) {
        HDC hdc = GetDC(hwnd);
        hBufferDC = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);
        hBufferBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(hBufferDC, hBufferBitmap);
        ReleaseDC(hwnd, hdc);

        m_framework = new Framework(NULL, hBufferBitmap, hBufferDC,&sock);
        if (!m_framework) {
            MessageBox(hwnd, L"Failed to create Framework instance", L"Error", MB_OK);
            return -1;
        }
        m_framework->m_hwnd = hwnd;
 
        return 0;
    }
    if (uMsg == WM_QUIT) {
        // 7. 소켓 종료
        closesocket(sock);
        WSACleanup();

    }
    if (m_framework) {
        return m_framework->windowproc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


DWORD WINAPI client_render(LPVOID param) {
    HWND hwnd = (HWND)param;

    auto previousTime = std::chrono::high_resolution_clock::now();
    const double frameDuration = 1.0 / 30.0; // 30 FPS

    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime >= frameDuration) {
            previousTime = currentTime;
            m_framework->render(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
}

DWORD WINAPI client_update(LPVOID param) {
    HWND hwnd = (HWND)param;

    auto previousTime = std::chrono::high_resolution_clock::now();
    const double frameDuration = 1.0 / 30.0; // 30 FPS

    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime >= frameDuration) {
            previousTime = currentTime;
            m_framework->update();
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    return 0;
}

