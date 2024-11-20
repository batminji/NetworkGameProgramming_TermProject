#include "Lobby_Scene.h"

Lobby_Scene::Lobby_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
	m_sock = sock;

	main_screen = &ResourceManager::getInstance().main_screen;
	start_button = &ResourceManager::getInstance().start_button;

	// 6. 랭킹 받기
	char recvBuf[BUFSIZE];
	int recvLen = recv(*m_sock, recvBuf, sizeof(SC_RANKING_PACKET), 0);
	if (recvLen <= 0) {
		std::cerr << "랭킹 받기 실패" << std::endl;
	}
	else {
		SC_RANKING_PACKET* resPacket = reinterpret_cast<SC_RANKING_PACKET*>(recvBuf);
		DataManager::getInstance().rank_arr[0].hs = resPacket->hs1;
		strcpy(DataManager::getInstance().rank_arr[0].id, resPacket->id1);

		DataManager::getInstance().rank_arr[1].hs = resPacket->hs2;
		strcpy(DataManager::getInstance().rank_arr[1].id, resPacket->id2);

		DataManager::getInstance().rank_arr[2].hs = resPacket->hs3;
		strcpy(DataManager::getInstance().rank_arr[2].id, resPacket->id3);

		DataManager::getInstance().rank_arr[3].hs = resPacket->hs4;
		strcpy(DataManager::getInstance().rank_arr[3].id, resPacket->id4);

		DataManager::getInstance().rank_arr[4].hs = resPacket->hs5;
		strcpy(DataManager::getInstance().rank_arr[4].id, resPacket->id5);

	}
}

void Lobby_Scene::render(LPVOID param)
{
	m_hwnd = (HWND)param;
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	//기본 배경출력
	main_screen->StretchBlt(m_hBufferDC, -20, -20, 800, 600, 0, 0, 800, 600, SRCCOPY);

	//아이템구매
	//for (int i = 0; i < 4; ++i)
	//    if (item[i].buy) item_check.TransparentBlt(m_hBufferDC, item[i].rt.left, item[i].rt.top - 10, item[i].rt.right - item[i].rt.left, item[i].rt.bottom - item[i].rt.top, 0, 0, 24, 24, RGB(255, 0, 255));

	//내정보

	WCHAR* text_out_id = new WCHAR[20];
	WCHAR* text_out_score = new WCHAR[20];
	WCHAR* text_out_coin = new WCHAR[20];
	MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().my_data.ID, -1, text_out_id, sizeof(DataManager::getInstance().my_data.ID));

	SetBkMode(m_hBufferDC, TRANSPARENT);
	TextOut(m_hBufferDC, 155, 65, text_out_id, lstrlen(text_out_id));
	wsprintf(text_out_score, L"%d", DataManager::getInstance().my_data.high_score);
	SetBkMode(m_hBufferDC, TRANSPARENT);
	TextOut(m_hBufferDC, 270, 90, text_out_score, lstrlen(text_out_score));
	wsprintf(text_out_coin, L"%d G", DataManager::getInstance().my_data.coin);
	SetBkMode(m_hBufferDC, TRANSPARENT);
	TextOut(m_hBufferDC, 555, 100, text_out_coin, lstrlen(text_out_coin));

	//방 참가하기 아이디 입력
	SetBkMode(m_hBufferDC, TRANSPARENT);
	TextOut(m_hBufferDC, 560, 528, join_room_id, lstrlen(join_room_id));

	// 메모리 해제
	delete[] text_out_id;
	delete[] text_out_score;
	delete[] text_out_coin;

	//랭킹
	WCHAR* number_text = new WCHAR[20];

	for (int i = 0; i < 5; i++) {
		WCHAR* user_name = new WCHAR[20];
		MultiByteToWideChar(CP_ACP, 0, DataManager::getInstance().rank_arr[i].id, -1, user_name, sizeof(DataManager::getInstance().rank_arr[i].id));

		SetBkMode(m_hBufferDC, TRANSPARENT);
		TextOut(m_hBufferDC, 158, (60 * i) + 205, user_name, lstrlen(user_name));

		wsprintf(number_text, L"%d", DataManager::getInstance().rank_arr[i].hs);
		SetBkMode(m_hBufferDC, TRANSPARENT);
		TextOut(m_hBufferDC, 158, (60 * i) + 225, number_text, lstrlen(number_text));

		// 메모리 해제
		delete[] user_name;
	}

	// 메모리 해제
	delete[] number_text;
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
	case WM_CHAR:
	{
		if (wParam == VK_BACK) {
			if (input_cnt == 0);
			else {
				join_room_id[input_cnt - 1] = NULL;
				input_cnt--;
			}
		}
		else if (input_cnt < 20) join_room_id[input_cnt++] = wParam;

	}
	break;
	case WM_LBUTTONDOWN:
	{
		int mx = LOWORD(lParam);
		int my = HIWORD(lParam);
		POINT mypt = { mx,my };
		if (PtInRect(&create_room_button, mypt)) { //create room
			printf("방만들귀\n");
			//방만들기를 눌렀을때 처리
			DataManager::getInstance().room_master = true;
			strcpy(DataManager::getInstance().ROOM_ID, DataManager::getInstance().my_data.ID); //참가 방아이디를 내아이디로
			next_scene = ROOM_SCENE;
		}
		if (PtInRect(&join_room_button, mypt)) { //join room
			printf("방참가하귀\n");
			//방 참가하기를 눌렀을때 처리
			DataManager::getInstance().room_master = false;
			WideCharToMultiByte(CP_ACP, 0, join_room_id, -1, DataManager::getInstance().ROOM_ID, sizeof(DataManager::getInstance().ROOM_ID), NULL, NULL);
			next_scene = ROOM_SCENE;
		}
	}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
