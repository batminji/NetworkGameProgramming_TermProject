#pragma once
#include "Scene.h"
class Title_Scene :
    public Scene
{

public:

    void render(LPVOID param) override;
    void update() override;
    LRESULT CALLBACK windowproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

