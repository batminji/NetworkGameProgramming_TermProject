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
    resourceManager.init(hInstance); // hInstance�� �����Ͽ� �ʱ�ȭ

   

   
    HWND hwnd = CreateWindowEx(
        0,                    // dwExStyle: Ȯ�� â ��Ÿ�� (������ 0)
        CLASS_NAME,            // Ŭ���� �̸�
        L"Kirby_don't_overeating",       // ������ �̸�
        WS_OVERLAPPEDWINDOW,  // ������ ��Ÿ��
        0, 0,                 // ��ġ (x, y)
        800, 600,             // ũ�� (width, height)
        NULL,                 // �θ� ������ (������ NULL)
        (HMENU)NULL,          // �޴� (������ NULL)
        hInstance,            // ���ø����̼� �ν��Ͻ� �ڵ�
        NULL                  // �߰����� ������ (������ NULL)
    );

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

    // ���� ���۸��� ����� �޸� DC���� �۾�
    while (isRunning) {
        
        m_framework->render(hwnd);
        // ȭ�� ���� ��û
        InvalidateRect(hwnd, NULL, FALSE);

        // ������ �ӵ� ���� (��: 60fps)
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
        // ȭ�� ���� ��û
        InvalidateRect(hwnd, NULL, FALSE);
        // ������ �ӵ� ���� (��: 60fps)
        Sleep(16); // milliseconds
    }
    return 0;
}
