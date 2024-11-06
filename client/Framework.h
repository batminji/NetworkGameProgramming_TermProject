#pragma once
#include "Scene.h"
#include "Title_Scene.h"


#include "test_scene.h"


class Framework
{

public:
	Scene* m_scene;
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;
public:
	Framework(HWND hwnd ,HBITMAP hBufferBitmap, HDC hBufferDC) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
		m_scene = new test_scene(m_hwnd,m_hBufferBitmap, m_hBufferDC);
	}
public:
	void render(LPVOID param);

	void update();

	LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};
