#include "Lobby_Scene.h"

Lobby_Scene::Lobby_Scene(HWND hwnd, HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock)
{
	m_hwnd = hwnd;
	m_hBufferBitmap = hBufferBitmap;
	m_hBufferDC = hBufferDC;
	m_sock = sock;

	main_screen = &ResourceManager::getInstance().main_screen;
	item_check = &ResourceManager::getInstance().item_check;

	DataManager::getInstance().my_item[0].rt = { 661, 175, 689, 202 };
	DataManager::getInstance().my_item[0].buy = false;
	DataManager::getInstance().my_item[0].buy_cnt = 0;
	DataManager::getInstance().my_item[0].price = 500;

	DataManager::getInstance().my_item[1].rt = { 661, 224, 689, 251 };
	DataManager::getInstance().my_item[1].buy = false;
	DataManager::getInstance().my_item[1].buy_cnt = 0;
	DataManager::getInstance().my_item[1].price = 1000;

	DataManager::getInstance().my_item[2].rt = { 661, 272, 689, 299 };
	DataManager::getInstance().my_item[2].buy = false;
	DataManager::getInstance().my_item[2].buy_cnt = 0;
	DataManager::getInstance().my_item[2].price = 500;

	result = System_Create(&ssystem);
	if (result != FMOD_OK)
		exit(0);
	ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	ssystem->createSound("sound_file/main_bgm.OGG", FMOD_LOOP_NORMAL, 0, &main_bgm);
	ssystem->createSound("sound_file/click.OGG", FMOD_DEFAULT, 0, &click_sound);
	ssystem->createSound("sound_file/Shop.OGG", FMOD_DEFAULT, 0, &shop_sound);
	ssystem->createSound("sound_file/buzzer.OGG", FMOD_DEFAULT, 0, &cant_shop_sound);
	ssystem->createSound("sound_file/cancle.OGG", FMOD_DEFAULT, 0, &shop_cancle_sound);
	ssystem->playSound(main_bgm, 0, false, &channel);
}

void Lobby_Scene::render(LPVOID param)
{
	m_hwnd = (HWND)param;
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	//기본 배경출력
	main_screen->StretchBlt(m_hBufferDC, -20, -20, 800, 600, 0, 0, 800, 600, SRCCOPY);

	//아이템구매
	for (int i = 0; i < 3; ++i)
	    if (DataManager::getInstance().my_item[i].buy) item_check->TransparentBlt(m_hBufferDC, DataManager::getInstance().my_item[i].rt.left, DataManager::getInstance().my_item[i].rt.top - 10, DataManager::getInstance().my_item[i].rt.right - DataManager::getInstance().my_item[i].rt.left, DataManager::getInstance().my_item[i].rt.bottom - DataManager::getInstance().my_item[i].rt.top, 0, 0, 24, 24, RGB(255, 0, 255));

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

void Lobby_Scene::network()
{
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
			ssystem->playSound(click_sound, 0, false, &channel);
			printf("방을 만들었습니다.\n");
			//방만들기를 눌렀을때 처리
			DataManager::getInstance().room_master = true;
			strcpy(DataManager::getInstance().ROOM_ID, DataManager::getInstance().my_data.ID); //참가 방아이디를 내아이디로

			CS_JOIN_ROOM_PACKET roomPacket;
			roomPacket.size = sizeof(CS_JOIN_ROOM_PACKET);
			roomPacket.type = CS_JOIN_ROOM;
			strcpy(roomPacket.id, DataManager::getInstance().ROOM_ID);
			for (int ii = 0; ii < 3; ++ii) roomPacket.item[ii] = DataManager::getInstance().my_item[ii].buy;

			if (send(*m_sock, reinterpret_cast<char*>(&roomPacket), sizeof(roomPacket), 0) == SOCKET_ERROR) {
				std::cerr << "Send failed." << std::endl;
				closesocket(*m_sock);
				WSACleanup();
			}
			next_scene = ROOM_SCENE;
			if(main_bgm)main_bgm->release();
			if (click_sound)click_sound->release();
			if (shop_sound)shop_sound->release();
			if (cant_shop_sound)cant_shop_sound->release();
			if (shop_cancle_sound)shop_cancle_sound->release();
			if (ssystem) {
				ssystem->close();
				ssystem->release();
			}
		}
		if (PtInRect(&join_room_button, mypt)) { //join room
			ssystem->playSound(click_sound, 0, false, &channel);
			printf("기존 방에 참가합니다.\n");
			//방 참가하기를 눌렀을때 처리
			DataManager::getInstance().room_master = false;
			WideCharToMultiByte(CP_ACP, 0, join_room_id, -1, DataManager::getInstance().ROOM_ID, sizeof(DataManager::getInstance().ROOM_ID), NULL, NULL);
			//참가할게~보내기
			CS_JOIN_ROOM_PACKET roomPacket;
			roomPacket.size = sizeof(CS_JOIN_ROOM_PACKET);
			roomPacket.type = CS_JOIN_ROOM;
			strcpy(roomPacket.id, DataManager::getInstance().ROOM_ID);

			if (send(*m_sock, reinterpret_cast<char*>(&roomPacket), sizeof(roomPacket), 0) == SOCKET_ERROR) {
				std::cout << "방참가 Send failed." << std::endl;
				closesocket(*m_sock);
				WSACleanup();
			}

			//이 방정보를 받기 
			char recvBuf[BUFSIZE];
			ZeroMemory(recvBuf, sizeof(recvBuf));


			int recvLen = recv(*m_sock, recvBuf, sizeof(SC_ROOM_CHANGE_PACKET), MSG_WAITALL);
			if (recvLen != sizeof(SC_ROOM_CHANGE_PACKET)) {
				std::cerr << "Receive failed or connection closed." << std::endl;
			}
			else {
				SC_ROOM_CHANGE_PACKET* roomPacket = reinterpret_cast<SC_ROOM_CHANGE_PACKET*>(recvBuf);
				std::cout <<"참가한 방 ID: "<< roomPacket->other_pl << std::endl;
				if (strcmp(roomPacket->other_pl, "cant_join")==0) { 
					MessageBox(
						NULL,
						L"없는 방인데요?",
						L"방참가 실패",
						MB_OK
					);
					return 0;
				}
			}

			
			next_scene = ROOM_SCENE;

			if (main_bgm)main_bgm->release();
			if (click_sound)click_sound->release();
			if (shop_sound)shop_sound->release();
			if (cant_shop_sound)cant_shop_sound->release();
			if (shop_cancle_sound)shop_cancle_sound->release();
			if (ssystem) {
				ssystem->close();
				ssystem->release();
			}
		}
		for (int i = 0; i < 3; ++i) {
			if (PtInRect(&DataManager::getInstance().my_item[i].rt, mypt)) {
				if (DataManager::getInstance().my_item[i].buy_cnt % 2 == 0) {
					if (DataManager::getInstance().my_item[i].price <= DataManager::getInstance().my_data.coin) {
						DataManager::getInstance().my_item[i].buy = TRUE;
						DataManager::getInstance().my_data.coin -= DataManager::getInstance().my_item[i].price;
						DataManager::getInstance().my_item[i].buy_cnt = (DataManager::getInstance().my_item[i].buy_cnt+1)%2;
					}
					else;
				}
				else {
					DataManager::getInstance().my_item[i].buy = FALSE;
					DataManager::getInstance().my_data.coin += DataManager::getInstance().my_item[i].price;
					DataManager::getInstance().my_item[i].buy_cnt = (DataManager::getInstance().my_item[i].buy_cnt + 1) % 2;
				}
			}
		}
	}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
