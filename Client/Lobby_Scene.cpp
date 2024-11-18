#include "Lobby_Scene.h"

Lobby_Scene::Lobby_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
	m_sock = sock;

	main_screen = &ResourceManager::getInstance().main_screen;
	start_button = &ResourceManager::getInstance().start_button;


}

void Lobby_Scene::render(LPVOID param)
{
    m_hwnd = (HWND)param;
    RECT rect;
    GetClientRect(m_hwnd, &rect);

	main_screen->StretchBlt(m_hBufferDC, 0, 0, 800, 600, 0, 0, 800, 600, SRCCOPY);
	start_button->TransparentBlt(m_hBufferDC, 10 - stbt_x, 10 - stbt_y, 800, 600, 0, 0, 800, 600, RGB(255, 0, 255));
	
	//아이템구매
	//for (int i = 0; i < 4; ++i)
	//	if (item[i].buy) item_check.TransparentBlt(m_hBufferDC, item[i].rt.left, item[i].rt.top - 10, item[i].rt.right - item[i].rt.left, item[i].rt.bottom - item[i].rt.top, 0, 0, 24, 24, RGB(255, 0, 255));
	
	//내정보
	//SetBkMode(m_hBufferDC, TRANSPARENT);
	//TextOut(m_hBufferDC, 155, 75, My.name, lstrlen(My.name));
	//wsprintf(number_text, L"%d", My.score);
	//SetBkMode(m_hBufferDC, TRANSPARENT);
	//TextOut(m_hBufferDC, 270, 100, number_text, lstrlen(number_text));
	//wsprintf(number_text, L"%d G", My.money);
	//SetBkMode(m_hBufferDC, TRANSPARENT);
	//TextOut(m_hBufferDC, 555, 110, number_text, lstrlen(number_text));
	
	//랭킹
	//for (int i = 0; (i < 5) && (i < all_data.size()); i++) {
	//	if (i < 3) {
	//		SetBkMode(m_hBufferDC, TRANSPARENT);
	//		TextOut(m_hBufferDC, 158, (60 * i) + 210, all_data[i].name, lstrlen(all_data[i].name));
	//		wsprintf(number_text, L"%d", all_data[i].score);
	//		SetBkMode(m_hBufferDC, TRANSPARENT);
	//		TextOut(m_hBufferDC, 158, (60 * i) + 230, number_text, lstrlen(number_text));
	//	}
	//	else {
	//		SetBkMode(m_hBufferDC, TRANSPARENT);
	//		TextOut(m_hBufferDC, 158, (60 * i) + 200, all_data[i].name, lstrlen(all_data[i].name));
	//		wsprintf(number_text, L"%d", all_data[i].score);
	//		SetBkMode(m_hBufferDC, TRANSPARENT);
	//		TextOut(m_hBufferDC, 158, (60 * i) + 220, number_text, lstrlen(number_text));
	//	}
	//}
}

void Lobby_Scene::update()
{
	if (stbt_direction) {
		stbt_x += 2; stbt_y += 3;
		if (stbt_x == 8)stbt_direction = FALSE;
	}
	else {
		stbt_x -= 2; stbt_y -= 3;
		if (stbt_x == 0)stbt_direction = TRUE;
	}
}

LRESULT Lobby_Scene::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_PAINT:
    {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0, 0, 800, 600, m_hBufferDC, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
        return 0;
    }
	case WM_LBUTTONDOWN:
	{
		int mx = LOWORD(lParam);
		int my = HIWORD(lParam);
		POINT mypt = { mx,my };
		if (PtInRect(&stbt_rt, mypt)) { // start
			next_scene = PLAY_SCENE;
		}
	}
		break;
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
