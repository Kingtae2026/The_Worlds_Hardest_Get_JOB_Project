#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <conio.h>
#include <chrono>
#include <vector>
#include "Types.h"
#include "GameSystem.h"
#include "MapManager.h"
#include "MainMenu.h"

// ── 게임 오버 화면 ────────────────────────────────────────────────────
static void ShowGameOver(MapManager& mapMgr) {
    ScreenBuffer& sb = mapMgr.GetBuffer();

    sb.FillRect(75, 18, 90, 17, L' ', ATTR(CC_WHITE, CC_DKRED));

    // 테두리
    for (int x = 75; x < 165; ++x) {
        sb.PutW(x, 18, L'=', ATTR(CC_YELLOW, CC_DKRED));
        sb.PutW(x, 18 + 16, L'=', ATTR(CC_YELLOW, CC_DKRED));
    }
    for (int y = 18; y < 18 + 17; ++y) {
        sb.PutW(75, y, L'|', ATTR(CC_YELLOW, CC_DKRED));
        sb.PutW(164, y, L'|', ATTR(CC_YELLOW, CC_DKRED));
    }

    sb.PutStrCtrW(21, L"[ G A M E  O V E R ]", ATTR(CC_WHITE, CC_DKRED));
    sb.PutStrCtrW(24, L"15번의 취업 실패...", ATTR(CC_YELLOW, CC_DKRED));
    sb.PutStrCtrW(26, L"멘탈이 붕괴되었습니다.", ATTR(CC_WHITE, CC_DKRED));
    sb.PutStrCtrW(30, L"(아무 키나 누르면 메뉴로 돌아갑니다)", ATTR(CC_GRAY, CC_DKRED));

    sb.Flip();
    Sleep(1000);
    while (_kbhit()) _getch();
    _getch();
}

// ── 게임 클리어 화면 ──────────────────────────────────────────────────
static void ShowGameClear(MapManager& mapMgr) {
    ScreenBuffer& sb = mapMgr.GetBuffer();

    sb.FillRect(60, 12, 120, 28, L' ', ATTR(CC_BLACK, CC_YELLOW));

    for (int x = 60; x < 180; ++x) {
        sb.PutW(x, 12, L'*', ATTR(CC_WHITE, CC_YELLOW));
        sb.PutW(x, 12 + 27, L'*', ATTR(CC_WHITE, CC_YELLOW));
    }
    for (int y = 12; y < 12 + 28; ++y) {
        sb.PutW(60, y, L'*', ATTR(CC_WHITE, CC_YELLOW));
        sb.PutW(179, y, L'*', ATTR(CC_WHITE, CC_YELLOW));
    }

    sb.PutStrCtrW(17, L"★ CONGRATULATIONS ★", ATTR(CC_DKRED, CC_YELLOW));
    sb.PutStrCtrW(20, L"[ 취업 성공! ]", ATTR(CC_BLACK, CC_YELLOW));
    sb.PutStrCtrW(23, L"당신은 결국 취업에 성공했습니다!", ATTR(CC_BLACK, CC_YELLOW));
    sb.PutStrCtrW(26, L"포트폴리오 1줄 추가 완료.", ATTR(CC_DKGRAY, CC_YELLOW));

    wchar_t deathMsg[128];
    swprintf(deathMsg, 128, L"총 사망 횟수: %d 회", mapMgr.GetTotalDeaths());
    sb.PutStrCtrW(30, deathMsg, ATTR(CC_DKRED, CC_YELLOW));
    sb.PutStrCtrW(34, L"(아무 키나 누르면 메뉴로 돌아갑니다)", ATTR(CC_DKGRAY, CC_YELLOW));

    sb.Flip();
    Sleep(1500);
    while (_kbhit()) _getch();
    _getch();
}

int main() {
    // ── 콘솔 설정 ────────────────────────────────────────────────────
    system("mode con cols=270 lines=55");
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    COORD bufferSize = { 270, 55 };
    SetConsoleScreenBufferSize(hOut, bufferSize);

    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
    DWORD dwInMode = 0;
    if (GetConsoleMode(hIn, &dwInMode)) {
        dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
        dwInMode |= ENABLE_EXTENDED_FLAGS;
        SetConsoleMode(hIn, dwInMode);
    }

    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);

    CONSOLE_FONT_INFOEX fontInfo = { sizeof(fontInfo) };
    GetCurrentConsoleFontEx(hOut, FALSE, &fontInfo);
    wcscpy_s(fontInfo.FaceName, L"Malgun Gothic");
    fontInfo.dwFontSize.Y = 19;
    SetCurrentConsoleFontEx(hOut, FALSE, &fontInfo);

    // ── 전체 게임 루프 (메뉴 ↔ 게임) ────────────────────────────────
    while (true) {
        MainMenu menu;
        MainMenu::MenuResult menuResult = menu.Show(hOut);
        if (menuResult == MainMenu::MenuResult::EXIT) break;

        system("cls");

        MapManager mapMgr;
        mapMgr.InitBuffer(hOut);
        mapMgr.LoadStage(6); // ★ 항상 1스테이지부터 시작

        bool isPlaying = true;
        auto startTime = std::chrono::steady_clock::now();

        const int TARGET_FPS = 40;
        const int MS_PER_FRAME = 1000 / TARGET_FPS;

        while (isPlaying) {
            auto frameStart = std::chrono::steady_clock::now();

            // ── 입력 ─────────────────────────────────────────────────
            if (_kbhit()) {
                int key = _getch();
                if (key == 27) {          // ESC → 메뉴로
                    isPlaying = false;
                    continue;
                }
                if (key == 224) {
                    key = _getch();
                    if (key == 72) mapMgr.MovePlayer(-1, 0); // 상
                    if (key == 80) mapMgr.MovePlayer(1, 0); // 하
                    if (key == 75) mapMgr.MovePlayer(0, -1); // 좌
                    if (key == 77) mapMgr.MovePlayer(0, 1); // 우
                }
            }

            // ── 장애물 업데이트 (내부에서 충돌 시 gs.OnPlayerDeath 호출) ─
            mapMgr.UpdateObstacles();

            // ── ★ GameSystem 상태 기반 분기 ──────────────────────────
            GameSystem::GameState gs = mapMgr.GetGameSystem().GetState();

            // 1. 엔딩 포탈 도달 체크 (RETURN_TO_MENU 상태가 되었을 때)
            if (gs == GameSystem::GameState::RETURN_TO_MENU) {
                auto now = std::chrono::steady_clock::now();
                int finalSec = (int)std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                mapMgr.GetGameSystem().SaveToHallOfFame(finalSec);
                isPlaying = false;
                continue;
            }

            if (gs == GameSystem::GameState::GAME_OVER) {
                mapMgr.RenderMap(); // 마지막 맵 화면 유지
                ShowGameOver(mapMgr);
                isPlaying = false;
                continue;
            }

            if (gs == GameSystem::GameState::GAME_CLEAR) {
                mapMgr.RenderMap();
                ShowGameClear(mapMgr);
                isPlaying = false;
                continue;
            }

            // ── 타이머 ───────────────────────────────────────────────
            auto now = std::chrono::steady_clock::now();
            int sec = (int)std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();

            // ── 렌더링 ───────────────────────────────────────────────
            mapMgr.RenderMap();

            // 상단 UI 오버레이 (TIME + STAGE DEATHS / 15)
            wchar_t uiBuf[128];
            int sd = mapMgr.GetStageDeaths();
            swprintf(uiBuf, 128, L" TIME: %02d:%02d  |  STAGE DEATHS: %d / %d ",
                sec / 60, sec % 60, sd, GameSystem::MAX_STAGE_DEATHS);
            WORD uiColor = (sd >= 10) ? ATTR(CC_WHITE, CC_DKRED) : ATTR(CC_YELLOW, CC_DKGRAY);
            mapMgr.GetBuffer().PutStrCtrW(0, uiBuf, uiColor);

            mapMgr.GetBuffer().Flip();

            // ── 프레임 대기 ──────────────────────────────────────────
            auto frameEnd = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
            if (elapsed.count() < MS_PER_FRAME)
                Sleep(MS_PER_FRAME - (int)elapsed.count());
        }
    }
    return 0;
}