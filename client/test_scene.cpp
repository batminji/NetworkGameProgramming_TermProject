#include "test_scene.h"

void test_scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    // ��� �׸���
    HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30)); // ����� ��ο� ������ ä���
    FillRect(m_hBufferDC, &rect, hBrush);
    DeleteObject(hBrush);

    // ĳ���� �׸��� (�ܼ��� �簢������ ǥ��)
    HBRUSH characterBrush = CreateSolidBrush(RGB(255, 0, 0)); // ĳ���� ����: ������
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
        // ���� �ʱ�ȭ
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
    case WM_KEYDOWN: // Ű �Է� ó��
        switch (wParam) {
        case 'W': characterY -= moveStep; break; // ���� �̵�
        case 'S': characterY += moveStep; break; // �Ʒ��� �̵�
        case 'A': characterX -= moveStep; break; // �������� �̵�
        case 'D': characterX += moveStep; break; // ���������� �̵�
        }
        InvalidateRect(hwnd, NULL, FALSE); // ȭ�� ���� ��û
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
