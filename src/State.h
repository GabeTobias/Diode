#pragma once

enum AppState
{
	GameMode = 0,
	DebugMode = 1,
	EditMode = 2
};

namespace GameState
{
	inline AppState Mode = AppState::GameMode;
	inline bool ShowOverlaps = false;
};
