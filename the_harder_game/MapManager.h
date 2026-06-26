#pragma once
#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#define LH 50
#define LW 120
#define OBS_COUNT 255
#define ROT_OBS_COUNT 400

#include "Types.h"
#include "ScreenBuffer.h"
#include "GameSystem.h"     // ★ 추가
#include <cmath>
#include <vector>

// 맵 타일 종류 정의
enum TileType {
    _W = 0, _L = 1, _S = 2, _E = 3,
    _K1 = 4, _K2 = 5, _K3 = 6, _K4 = 7, _K5 = 8,
    _D1 = 9, _D2 = 10, _D3 = 11, _D4 = 12, _D5 = 13
};

// 일반 직선 왕복 장애물
struct Obs {
    int r, c, dr, dc;
    int minR, maxR, minC, maxC;
    int speed;
    int tick;
};

// 회전 장애물
struct RotObs {
    float cx, cy;
    float radius;
    float angle;
    float speed;
    int r, c;
};

// 스테이지 정보
struct StageInfo {
    const char* name;
    const char* worldName;
    int level;
};

// 체이서 구조체
struct Chaser {
    float r, c;
    float speed;
    bool active;
    int randomDR = 0;  // ★ 추가
    int randomDC = 1;  // ★ 추가 (초기 방향 오른쪽)
};


class MapManager {
private:
    // 1. 맵 데이터 관련
    int currentStageNum;
    int (*mapData)[LW];
    static int stage1[LH][LW];
    static int stage2[LH][LW];
    static int stage3[LH][LW];
    static int stage4[LH][LW];
    static int stage5[LH][LW];
    static int stage6[LH][LW];
    // 2. 플레이어 및 게임 진행 상태
    bool hasKey[6];
    int playerR, playerC;
    bool hasKey1, hasKey2;
    int deaths;
    int stage6HorizonR; // 스테이지6 진입 시 고정된 하늘/땅 경계 행
    // 3. 장애물 관리
    Obs    obsList[OBS_COUNT];
    RotObs rotObsList[ROT_OBS_COUNT];
    int    obsCount;
    int    rotObsCount;

    // 4. 렌더링
    ScreenBuffer sb;

    // 5. ★ 게임 상태/사망 카운터 (GameSystem 에 위임)
    GameSystem gs;

    // 6. 은비 - 쫓아오는 장애물 추가
#define CHASER_COUNT 5
    Chaser chaserList[CHASER_COUNT];
    bool stage4RightEntered = false;
    int chaserRandomTick = 0;

public:
    MapManager();

    // 초기화 및 로드
    void InitBuffer(HANDLE hOut);
    void LoadStage(int stageNum);
    // 엔딩 문구를 출력하기 위한 함수
    void PrintEndingMessage();
    // 업데이트 및 로직
    void UpdateObstacles();
    void MovePlayer(int dr, int dc);
    void OpenDoorAnimation(int doorType);

    // 렌더링
    void RenderMap();

    // 유틸리티/조회
    bool CanMove(int r, int c);
    int  GetTile(int r, int c) const;

    ScreenBuffer& GetBuffer() { return sb; }

    // ★ GameSystem 접근자 (main.cpp 에서 상태/사망 수 조회용)
    GameSystem& GetGameSystem() { return gs; }

    // 하위 호환용 래퍼 (기존 main.cpp 호출 그대로 사용 가능)
    int GetTotalDeaths() const { return gs.GetTotalDeaths(); }
    int GetStageDeaths() const { return gs.GetStageDeaths(); }

    void UpdatePlayerState(int r, int c, bool k1, bool k2) {
        playerR = r; playerC = c; hasKey1 = k1; hasKey2 = k2;
    }
    void DrawPixelCloud(int r, int c, bool isBig);
    void VerticalOpenAnimation(int startR, int startC);

};

#endif