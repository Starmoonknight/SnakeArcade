// GameRenderer.h
#pragma once
#include <vector>
#include <string>

struct GameConfig;
class GameGrid;


namespace GameRender
{
	struct BlockLayout
	{
		int originX{};
		int originY{};
		int width{};
		int height{};
	};

	void ClearConsole(); 

	void SetCursorPosition(int x, int y);
	void HideCursor();
	void ShowCursor();

	BlockLayout RenderFollowupBlock(const BlockLayout& previousLayout, const std::vector<std::string>& lines);
	void RenderPressEnterPrompt(const BlockLayout layout);
	BlockLayout RenderMainMenu();
	BlockLayout RenderHelpMenu();
	BlockLayout RenderScoreBoard(int highScore);
	BlockLayout RenderGameOver(int score, int highScore);

	void RenderPlaying(const GameConfig& cfg, const GameGrid& grid, int score); 
}