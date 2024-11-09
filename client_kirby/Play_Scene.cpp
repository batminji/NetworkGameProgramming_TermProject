#include "Play_Scene.h"

void Play_Scene::render(LPVOID param)
{
	m_hwnd = (HWND)param;
	RECT rect;
	GetClientRect(m_hwnd, &rect);
    // ��� �׸���
    HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30)); // ����� ��ο� ������ ä���
    FillRect(m_hBufferDC, &rect, hBrush);
    DeleteObject(hBrush);

    // ĳ���� �׸��� (�ܼ��� �簢������ ǥ��)
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
