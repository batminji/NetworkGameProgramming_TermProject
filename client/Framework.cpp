#include "Framework.h"

Framework::~Framework()
{
	if (m_scene) delete m_scene;
}

void Framework::render(LPVOID param)
{
	std::lock_guard ll{ m_scene->scene_lock };
	if (m_scene != nullptr)
		m_scene->render(param);
}

void Framework::update()
{
	std::lock_guard ll{ m_scene->scene_lock };
	m_scene->update();

	if (current_scene != m_scene->next_scene) {
		current_scene = m_scene->next_scene;

		switch (m_scene->next_scene)
		{
		case TITLE_SCENE:
		{
			auto tmp = m_scene;
			m_scene = new Title_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = TITLE_SCENE;
			delete tmp;
		}
		break;
		case LOBBY_SCENE:
		{
			auto tmp = m_scene;
			m_scene = new Lobby_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = LOBBY_SCENE;
			delete tmp;
		}
		break;
		case ROOM_SCENE:
		{
			CS_JOIN_ROOM_PACKET roomPacket;
			roomPacket.size = sizeof(CS_JOIN_ROOM_PACKET);
			roomPacket.type = CS_JOIN_ROOM;
			strcpy(roomPacket.id, DataManager::getInstance().ROOM_ID);

			if (send(*m_sock, reinterpret_cast<char*>(&roomPacket), sizeof(roomPacket), 0) == SOCKET_ERROR) {
				std::cerr << "Send failed." << std::endl;
				closesocket(*m_sock);
				WSACleanup();
			}
			auto tmp = m_scene;
			if (DataManager::getInstance().room_master){
				m_scene = new Room_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, TRUE);
			}
			else m_scene = new Room_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, FALSE);

			m_scene->next_scene = ROOM_SCENE;
			delete tmp;
		}
		break;
		case PLAY_SCENE:
		{
			// Room_Scene���� �÷��̾� �����͸� ������
			/*Player master_player()
			Player join_player = *dynamic_cast<Room_Scene*>(m_scene)->getJoinPlayer();*/
			if (dynamic_cast<Room_Scene*>(m_scene)->master_player->who_is_me) {
				Player master_player(dynamic_cast<Room_Scene*>(m_scene)->master_player->job, TRUE);
				Player join_player(dynamic_cast<Room_Scene*>(m_scene)->join_player->job, FALSE);

				auto tmp = m_scene;
				m_scene = nullptr;
				m_scene = new Play_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, &master_player, &join_player);
				m_scene->next_scene = PLAY_SCENE;
				delete tmp;
			}
			else if(dynamic_cast<Room_Scene*>(m_scene)->join_player->who_is_me){
				Player master_player(dynamic_cast<Room_Scene*>(m_scene)->master_player->job, FALSE);
				Player join_player(dynamic_cast<Room_Scene*>(m_scene)->join_player->job, TRUE);

				auto tmp = m_scene;
				m_scene = nullptr;
				m_scene = new Play_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, &master_player, &join_player);
				m_scene->next_scene = PLAY_SCENE;
				delete tmp;
			}
		}
		break;
		case CARTOON_SCENE:
		{
			auto tmp = m_scene;
			m_scene = nullptr;
			m_scene = new Cartoon_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = CARTOON_SCENE;
			delete tmp;
		}
		break;
		default:
			break;
		}

	}
}

void Framework::network()
{
	std::lock_guard ll{ m_scene->scene_lock };
	if (m_scene != nullptr)
		m_scene->network();
}

LRESULT Framework::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CHAR) {

		if (wParam - 48 >= (char)TITLE_SCENE && wParam - 48 <= (char)CARTOON_SCENE)
			m_scene->next_scene = (short)(wParam - 48);
	}
	if (m_scene != nullptr)
		m_scene->windowproc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}