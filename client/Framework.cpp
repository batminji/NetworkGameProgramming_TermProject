#include "Framework.h"

Framework::~Framework()
{
	if (m_scene) delete m_scene;
}

void Framework::render(LPVOID param)
{
	if (m_scene)m_scene->render(param);
}

void Framework::update()
{

	m_scene->update();

	if (current_scene != m_scene->next_scene) {
		current_scene = m_scene->next_scene;

		switch (m_scene->next_scene)
		{
		case TITLE_SCENE:
		{
			delete m_scene;
			m_scene = new Title_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = TITLE_SCENE;
		}
		break;
		case LOBBY_SCENE:
		{
			delete m_scene;
			m_scene = new Lobby_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = LOBBY_SCENE;
		}
		break;
		case ROOM_SCENE:
		{
			delete m_scene;
			// 내가 방장일 경우
			m_scene = new Room_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, TRUE);
			// 내가 조인일 경우
			// m_scene = new Room_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, FALSE);
			m_scene->next_scene = ROOM_SCENE;
		}
		break;
		case PLAY_SCENE:
		{
			// Room_Scene에서 플레이어 데이터를 가져옴
			Player master_player = *dynamic_cast<Room_Scene*>(m_scene)->getMasterPlayer();
			Player join_player = *dynamic_cast<Room_Scene*>(m_scene)->getJoinPlayer();
			delete m_scene;
			m_scene = new Play_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock, &master_player, &join_player);
			m_scene->next_scene = PLAY_SCENE;
		}
		break;
		case CARTOON_SCENE:
		{
			delete m_scene;
			m_scene = new Cartoon_Scene(m_hwnd, m_hBufferBitmap, m_hBufferDC, m_sock);
			m_scene->next_scene = CARTOON_SCENE;
		}
		break;
		default:
			break;
		}

	}
}

LRESULT Framework::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CHAR) {

		if (wParam - 48 >= (char)TITLE_SCENE && wParam - 48 <= (char)CARTOON_SCENE)
			m_scene->next_scene = (short)(wParam - 48);
	}
	if (m_scene)m_scene->windowproc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}