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


//��Ʈ��ũ
const wchar_t* SERVER_IP = L"127.0.0.1";
WSADATA wsaData;
SOCKET sock;
SOCKADDR_IN serverAddr;


void CreateConsole()
{ // ���ο� �ܼ� â�� �Ҵ� 
    AllocConsole(); // �ܼ� ������� ǥ�� ��Ʈ���� ���� 
    FILE* fp; freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONERR$", "w", stderr);
    std::cout << "�ܼ� â�� �����Ǿ����ϴ�." << std::endl;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    CreateConsole(); // �޼���Ȯ�ο� �ܼ� ����
  // 1. ���� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }
   
    // 2. ���� ����
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return -1;
    }
   
    // 3. ���� �ּ� ����
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
   
    // InetPton���� IP �ּ� ����
    if (InetPton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }
   
    // 4. ������ ����
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
    resourceManager.init(hInstance); // hInstance�� �����Ͽ� �ʱ�ȭ

   

   
    HWND hwnd = CreateWindowEx(
        0,                    // dwExStyle: Ȯ�� â ��Ÿ�� (������ 0)
        CLASS_NAME,            // Ŭ���� �̸�
        L"Kirby_don't_overeating",       // ������ �̸�
        WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;
  

    ShowWindow(hwnd, nCmdShow);
    HANDLE Render_Thread = NULL;
    HANDLE Update_Thread = NULL;
    // ������ ������ ����
    if (m_framework) {
       Render_Thread = CreateThread(NULL, 0, client_render, hwnd, 0, NULL);
    }
    // ������Ʈ ������ ����
    if (m_framework) {
        Update_Thread = CreateThread(NULL, 0, client_update, hwnd, 0, NULL);
    }
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ������ ���� ����
    isRunning = false;
    WaitForSingleObject(Render_Thread, INFINITE); // �����尡 ����� ������ ���
    CloseHandle(Render_Thread); // ������ �ڵ� �ݱ�
    WaitForSingleObject(Update_Thread, INFINITE); // �����尡 ����� ������ ���
    CloseHandle(Update_Thread); // ������ �ڵ� �ݱ�

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
        // 7. ���� ����
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

