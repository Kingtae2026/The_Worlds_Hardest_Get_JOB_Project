#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H


// ════════════════════════════════════════════════════════════════════
//  GameSystem : 게임 상태 + 사망/스테이지 카운터 관리
//  (이전의 float 기반 CheckCollision/HandlePlayerDeath 는 제거됨)
//  실제 충돌 판정은 MapManager 가 그리드 좌표로 직접 수행하고,
//  결과(사망/클리어)를 GameSystem 에 위임합니다.
// ════════════════════════════════════════════════════════════════════
#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include <vector>     
#include <algorithm>  
class GameSystem {
public:
    // ── 게임 상태 ────────────────────────────────────────────────────
    enum class GameState {
        PLAYING,        // 정상 플레이 중
        PLAYER_DEAD,    // 사망 → 리스폰 처리 중  (현재 코드에서는 즉시 LoadStage 로 처리)
        GAME_OVER,      // 스테이지 사망 횟수 초과
        GAME_CLEAR,    // 최종 스테이지(5) 출구 도달 → 엔딩
        RETURN_TO_MENU
    };

    static const int MAX_STAGE_DEATHS = 15; // 스테이지당 허용 사망 수

private:
    GameState state;
    int totalDeaths;   // 전체 누적 사망
    int stageDeaths;   // 현재 스테이지 사망 (스테이지 전환 시 초기화)

public:
    GameSystem()
        : state(GameState::PLAYING)
        , totalDeaths(0)
        , stageDeaths(0) {
    }

    // ── 이벤트 ───────────────────────────────────────────────────────

    // 플레이어 사망 시 호출.
    //   반환값 true  → GAME_OVER (stageDeaths >= MAX_STAGE_DEATHS)
    //   반환값 false → 리스폰 가능
    bool OnPlayerDeath() {
        totalDeaths++;
        stageDeaths++;
        if (stageDeaths >= MAX_STAGE_DEATHS) {
            state = GameState::GAME_OVER;
            return true;
        }
        state = GameState::PLAYER_DEAD;
        return false;
    }

    // 스테이지 전환 시 호출 (스테이지 데스 리셋)
    void OnStageAdvance() {
        stageDeaths = 0;
        state = GameState::PLAYING;
    }

    // 최종 스테이지 클리어 (출구 도달) 시 호출
    void OnGameClear() {
        state = GameState::GAME_CLEAR;
    }

    // 리스폰 후 다시 PLAYING 으로 복귀
    void SetPlaying() {
        state = GameState::PLAYING;
    }

    void SetState(GameState newState) { state = newState; }

    void SaveToHallOfFame(int totalSeconds) {
        // ── 기존 기록 읽기 ──────────────────────────────────────────
        std::vector<int> records;
        std::ifstream inFile("rank.txt");
        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                if (line.size() >= 5) {
                    // "MM:SS" → 초 단위로 변환
                    int m = std::stoi(line.substr(0, 2));
                    int s = std::stoi(line.substr(3, 2));
                    records.push_back(m * 60 + s);
                }
            }
            inFile.close();
        }

        // ── 새 기록 추가 후 오름차순 정렬 ──────────────────────────
        records.push_back(totalSeconds);
        std::sort(records.begin(), records.end()); // 빠른 시간 순

        // ── 파일 덮어쓰기 ───────────────────────────────────────────
        std::ofstream outFile("rank.txt", std::ios::trunc); // 기존 내용 삭제 후 새로 씀
        if (outFile.is_open()) {
            for (int sec : records) {
                int m = sec / 60;
                int s = sec % 60;
                outFile << std::setfill('0') << std::setw(2) << m << ":"
                    << std::setfill('0') << std::setw(2) << s << "\n";
            }
            outFile.close();
        }
    }

    // ── 조회 ─────────────────────────────────────────────────────────
    GameState GetState()     const { return state; }
    int GetTotalDeaths()     const { return totalDeaths; }
    int GetStageDeaths()     const { return stageDeaths; }
};

#endif