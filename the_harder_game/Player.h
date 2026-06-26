#pragma once
#include "Types.h"

// 플레이어 초기화 및 이동 로직 담당
class PlayerManager
{
public:
	Player player;

	// 초기 위치 설정
	void Init(float startX, float startY);

	// 키보드 입력을 받아 위치 변경 (MapManager와 협력하여 벽 체크)
	void Update();

	// 플레이어를 화면에 그리는 함수
	void Render();

private:
	// 실제 이동 전 갈 수 있는 곳인지 체크하는 내부 로직
	bool CanMove(float nextX, float nextY);

	// 커서위치는 옮기는 함수
	void SetCursorPosition(int x, int y);
};