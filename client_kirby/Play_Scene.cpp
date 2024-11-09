#include "Play_Scene.h"

void Play_Scene::render(LPVOID param)
{
	m_hwnd = (HWND)param;
	RECT rect;
	GetClientRect(m_hwnd, &rect);
    // 배경 그리기
    HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30)); // 배경을 어두운 색으로 채우기
    FillRect(m_hBufferDC, &rect, hBrush);
    DeleteObject(hBrush);

    // 캐릭터 그리기 (단순한 사각형으로 표현)
    player->render(m_hBufferDC);
}

void Play_Scene::update()
{
}

LRESULT Play_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, m_hBufferDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
        switch (wParam) {
        default:break;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
