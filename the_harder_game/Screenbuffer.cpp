#define _CRT_SECURE_NO_WARNINGS
#include "ScreenBuffer.h"
#include <cstring>
#include <cwchar>

// ─────────────────────────────────────────────────────────────────────
//  CharWidth : 한글·CJK 전각 문자는 2칸, 나머지는 1칸
// ─────────────────────────────────────────────────────────────────────
int ScreenBuffer::CharWidth(wchar_t ch) {
    if (ch >= 0xAC00 && ch <= 0xD7A3) return 2; // 한글 음절
    if (ch >= 0x4E00 && ch <= 0x9FFF) return 2; // CJK 통합 한자
    if (ch >= 0x1100 && ch <= 0x11FF) return 2; // 한글 자모
    if (ch >= 0x3130 && ch <= 0x318F) return 2; // 한글 호환 자모
    if (ch >= 0x3000 && ch <= 0x303F) return 2; // CJK 기호
    if (ch >= 0xFF00 && ch <= 0xFFEF) return 2; // 전각 ASCII
    return 1;
}

int ScreenBuffer::DisplayWidth(const wchar_t* s) {
    int w = 0;
    for (const wchar_t* p = s; *p; ++p)
        w += CharWidth(*p);
    return w;
}

// ─────────────────────────────────────────────────────────────────────
void ScreenBuffer::Init(HANDLE h) {
    hOut = h;
    Clear();
}

void ScreenBuffer::Clear(WORD attr) {
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            cells[y][x].Char.UnicodeChar = L' ';
            cells[y][x].Attributes = attr;
        }
}

// ─────────────────────────────────────────────────────────────────────
//  PutW : 단일 문자 출력
//  한글처럼 2칸짜리 문자는 오른쪽 셀을 공백으로 예약합니다.
// ─────────────────────────────────────────────────────────────────────
int ScreenBuffer::PutW(int x, int y, wchar_t ch, WORD attr) {
    int w = CharWidth(ch);
    if (y < 0 || y >= H) return w;

    if (x >= 0 && x < W) {
        cells[y][x].Char.UnicodeChar = ch;
        cells[y][x].Attributes = attr;
    }
    // 2칸 문자: 오른쪽 셀을 같은 배경색의 공백으로 채움
    if (w == 2 && x + 1 >= 0 && x + 1 < W) {
        cells[y][x + 1].Char.UnicodeChar = L' ';
        cells[y][x + 1].Attributes = attr;
    }
    return w;
}

int ScreenBuffer::PutStrW(int x, int y, const wchar_t* s, WORD attr) {
    int cx = x;
    for (const wchar_t* p = s; *p; ++p)
        cx += PutW(cx, y, *p, attr);
    return cx - x;
}

void ScreenBuffer::PutStrCtrW(int y, const wchar_t* s, WORD attr) {
    int dw = DisplayWidth(s);
    int x = ((W - dw) / 2) - 15;
    if (x < 0) x = 0; // 화면 밖으로 나가지 않게 방어
    PutStrW(x, y, s, attr);
}

void ScreenBuffer::FillRect(int x, int y, int w, int h, wchar_t ch, WORD attr) {
    for (int row = y; row < y + h && row < H; ++row)
        for (int col = x; col < x + w && col < W; ++col) {
            cells[row][col].Char.UnicodeChar = ch;
            cells[row][col].Attributes = attr;
        }
}

void ScreenBuffer::FillRow(int y, WORD attr) {
    if (y < 0 || y >= H) return;
    for (int x = 0; x < W; ++x) {
        cells[y][x].Char.UnicodeChar = L' ';
        cells[y][x].Attributes = attr;
    }
}

// ─────────────────────────────────────────────────────────────────────
//  Flip : CHAR_INFO 배열 전체를 한 번에 화면에 출력 → 깜빡임 없음
// ─────────────────────────────────────────────────────────────────────
void ScreenBuffer::Flip() {
    COORD     bufSize = { (SHORT)W,     (SHORT)H };
    COORD     bufCoord = { 0,            0 };
    SMALL_RECT region = { 0, 0, (SHORT)(W - 1), (SHORT)(H - 1) };
    WriteConsoleOutputW(hOut, &cells[0][0], bufSize, bufCoord, &region);
}