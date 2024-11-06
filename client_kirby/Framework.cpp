#include "Framework.h"


void Framework::render(LPVOID param)
{
	m_scene->render(param);
}

void Framework::update()
{
	m_scene->update();
}

LRESULT Framework::windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_scene->windowproc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
