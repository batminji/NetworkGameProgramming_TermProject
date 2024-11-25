#pragma once
#include "stdafx.h"

class Scene
{
public:
	HWND m_hwnd;
	HBITMAP m_hBufferBitmap;
	HDC m_hBufferDC;
	SOCKET* m_sock;

	short next_scene;
public:
	std::mutex scene_lock;


	Scene() {};
	Scene(HWND hwnd,HBITMAP hBufferBitmap, HDC hBufferDC, SOCKET* sock) {
		m_hwnd = hwnd;
		m_hBufferBitmap = hBufferBitmap;
		m_hBufferDC = hBufferDC;
		m_sock = sock;
	};

public:
	bool running = 0;

	
public:
	//virtualÇÔ¼öµé

	virtual void render(LPVOID param) =0 ;

	virtual void update() = 0 ;

	virtual void network() = 0;

	virtual LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
		return DefWindowProc(hwnd, uMsg, wParam, lParam); }

};

