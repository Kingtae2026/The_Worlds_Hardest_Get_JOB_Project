#pragma once
#include <vector>
#include <windows.h>
#include "Types.h" // Point, Player, Obstacle 사용

class RenderManager {
private:
    HANDLE hConsole;
public:
    RenderManager();
    void SetCursor(int x, int y);
    // 맵 데이터(2차원 배열), 플레이어, 장애물 목록을 받아 한 번에 그립니다.
    void Draw(const std::vector<std::vector<int>>& map, const Player& player, const std::vector<Obstacle>& obstacles, int currentStage);
};