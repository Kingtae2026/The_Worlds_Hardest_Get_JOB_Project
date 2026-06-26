#pragma once
#ifndef SCREENBUFFER_H
#define SCREENBUFFER_H

#include <Windows.h>

enum CC {
    CC_BLACK = 0x0, CC_DKBLUE = 0x1, CC_DKGREEN = 0x2, CC_DKCYAN = 0x3,
    CC_DKRED = 0x4, CC_DKMAG = 0x5, CC_DKYEL = 0x6, CC_GRAY = 0x7,
    CC_DKGRAY = 0x8, CC_BLUE = 0x9, CC_GREEN = 0xA, CC_CYAN = 0xB,
    CC_RED = 0xC, CC_MAG = 0xD, CC_YELLOW = 0xE, CC_WHITE = 0xF
};

#define ATTR(fg, bg)  ((WORD)(((bg) << 4) | (fg)))

class ScreenBuffer {
public:
    static const int W = 270;
    static const int H = 55;   // ★ 버퍼 크기 270x55 에 맞춤

    void Init(HANDLE hOut);
    void Clear(WORD attr = ATTR(CC_WHITE, CC_BLACK));
    int  PutW(int x, int y, wchar_t ch, WORD attr);
    int  PutStrW(int x, int y, const wchar_t* s, WORD attr);
    void PutStrCtrW(int y, const wchar_t* s, WORD attr);
    void FillRect(int x, int y, int w, int h, wchar_t ch, WORD attr);
    void FillRow(int y, WORD attr);
    void Flip();

    static int CharWidth(wchar_t ch);
    static int DisplayWidth(const wchar_t* s);

private:
    HANDLE    hOut;
    CHAR_INFO cells[H][W];
};

#endif