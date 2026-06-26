#pragma once
// 공통 기반
struct Point {
    float x, y;
};

struct Player {
    Point pos;
    int deaths;
    bool isAlive;
};

struct Obstacle {
    Point pos;
    Point start; // 왕복 시작점
    Point end;   // 왕복 끝점
    float angle; // 회전형일 때 현재 각도
    float speed; // 움직임 속도
    int type;    // 0: 왕복, 1: 회전
};
