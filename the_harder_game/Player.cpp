#include<iostream>
#include<conio.h>
#include "Player.h"
#include<Windows.h>
using namespace std;

// 초기화 함수
void PlayerManager::Init(float startX, float startY) {
    player.pos = { startX, startY }; // 시작위치 설정
    player.deaths = 0;               // 죽은횟수 0으로 초기화
    player.isAlive = true;           // 살아있는 상태로 시작

    // 게임 시작 전 콘솔 커서를 숨깁니다.(깜빡임방지)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; // 커서 숨김
}

// 입력 처리 및 이동 함수
void PlayerManager::Update() {
    // 키보드 입력이 있는지 확인
    if (_kbhit()) {
        int key = _getch();
        float nextX = player.pos.x;
        float nextY = player.pos.y;

        // 방향설정
        switch (key) {
        case 'w': case 'W': nextY -= 1.0f; break; // 위
        case 's': case 'S': nextY += 1.0f; break; // 아래
        case 'a': case 'A': nextX -= 1.0f; break; // 왼쪽
        case 'd': case 'D': nextX += 1.0f; break; // 오른쪽
        default: return;
        }

        SetCursorPosition((int)player.pos.x, (int)player.pos.y);
        cout << "  "; // 공백 두개로 'P '를 지움
        if (CanMove(nextX, nextY)) {
            player.pos.x = nextX;
            player.pos.y = nextY;
        }
    }
}

void PlayerManager::Render() {
    // 플레이어의 새로운 위치로 커서를 이동
    SetCursorPosition((int)player.pos.x, (int)player.pos.y);

    // 플레이어 그리기
    cout << "P ";
}

bool PlayerManager::CanMove(float nextX, float nextY) 
{
// 화면 경계체크(임시 벽)
    if (nextX < 0 || nextY < 0 || nextX > 20 || nextY>20) return false;
    return true;
}

void PlayerManager::SetCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)x * 2, (SHORT)y }; // 폰트 비율에 맞춰 x는 2배
    SetConsoleCursorPosition(hConsole, coord);
}