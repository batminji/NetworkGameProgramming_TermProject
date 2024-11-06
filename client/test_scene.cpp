#include "test_scene.h"

void test_scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    // 배경 그리기
    HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30)); // 배경을 어두운 색으로 채우기
    FillRect(m_hBufferDC, &rect, hBrush);
    DeleteObject(hBrush);

    // 캐릭터 그리기 (단순한 사각형으로 표현)
    HBRUSH characterBrush = CreateSolidBrush(RGB(255, 0, 0)); // 캐릭터 색상: 빨간색
    RECT characterRect = { characterX, characterY, characterX + 20, characterY + 20 };
    FillRect(m_hBufferDC, &characterRect, characterBrush);
    DeleteObject(characterBrush);
}

void test_scene::update()
{
}

LRESULT test_scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
    {
        // 버퍼 초기화
        HDC hdc = GetDC(hwnd);
        m_hBufferDC = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);
        m_hBufferBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(m_hBufferDC, m_hBufferBitmap);
        ReleaseDC(hwnd, hdc);
    }
    return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
    return 0;
    case WM_KEYDOWN: // 키 입력 처리
        switch (wParam) {
        case 'W': characterY -= moveStep; break; // 위로 이동
        case 'S': characterY += moveStep; break; // 아래로 이동
        case 'A': characterX -= moveStep; break; // 왼쪽으로 이동
        case 'D': characterX += moveStep; break; // 오른쪽으로 이동
        }
        InvalidateRect(hwnd, NULL, FALSE); // 화면 갱신 요청
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
