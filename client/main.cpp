#pragma once
#include "stdafx.h"
#include "Framework.h"
#include "ResourceManager.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI client_render(LPVOID param);
DWORD WINAPI client_update(LPVOID param);

HBITMAP hBufferBitmap;
HDC hBufferDC;
bool isRunning = true;

Framework* m_framework;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
 
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
        WS_OVERLAPPEDWINDOW,  // 윈도우 스타일
        0, 0,                 // 위치 (x, y)
        800, 600,             // 크기 (width, height)
        NULL,                 // 부모 윈도우 (없으면 NULL)
        (HMENU)NULL,          // 메뉴 (없으면 NULL)
        hInstance,            // 애플리케이션 인스턴스 핸들
        NULL                  // 추가적인 데이터 (없으면 NULL)
    );

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

        m_framework = new Framework(NULL, hBufferBitmap, hBufferDC);
        if (!m_framework) {
            MessageBox(hwnd, L"Failed to create Framework instance", L"Error", MB_OK);
            return -1;
        }
        m_framework->m_hwnd = hwnd;
 
        return 0;
    }

    if (m_framework) {
        return m_framework->windowproc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI client_render(LPVOID param) {
    HWND hwnd = (HWND)param;

    // 더블 버퍼링에 사용할 메모리 DC에서 작업
    while (isRunning) {
        
        m_framework->render(hwnd);
        // 화면 갱신 요청
        InvalidateRect(hwnd, NULL, FALSE);

        // 프레임 속도 조절 (예: 60fps)
        Sleep(16); // milliseconds
    }
    return 0;
}

DWORD WINAPI client_update(LPVOID param)
{
    HWND hwnd = (HWND)param;
   
    while (isRunning)
    {
        m_framework->update();
        // 화면 갱신 요청
        InvalidateRect(hwnd, NULL, FALSE);
        // 프레임 속도 조절 (예: 60fps)
        Sleep(16); // milliseconds
    }
    return 0;
}
