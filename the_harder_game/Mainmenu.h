#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H

#include <Windows.h>
#include "ScreenBuffer.h"

class MainMenu {
public:
    enum class MenuResult { START_GAME, HOW_TO_PLAY, HALL_OF_FAME, EXIT };

    MainMenu();
    MenuResult Show(HANDLE hOut);   // ★ HANDLE 인자 추가

private:
    static const int DROP_CNT = 60;
    static const int MENU_CNT = 4;

    struct Drop { int x, y, spd, tick; wchar_t ch; };

    ScreenBuffer sb;
    int  sel, deaths, tagIdx, frame;
    bool titleDone;
    int  titleLine, titleDelay;
    Drop drops[DROP_CNT];

    void InitDrops();
    void RenderAll();
    void RenderMatrix();
    void RenderTitle();
    void RenderMenu();
    void RenderTagline();
    void RenderHints();
    void RenderDeathCounter();
    void RenderPopup(int idx);
    bool WaitPopupInput(int idx);
    void RenderHallOfFame();
    void RenderGuide();
};

#endif