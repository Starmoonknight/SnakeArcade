// GameRender.h
#pragma once


struct GameConfig;
class GameGrid;


namespace GameRender
{
	void ClearConsole(); 

	void SetCursorPosition(int x, int y);
	void HideCursor();
	void ShowCursor();

	void RenderMainMenu();
	void RenderHelpMenu();
	void RenderScoreBoard(int highScore);
	void RenderGameOver(int score, int highScore);

	void RenderPlaying(const GameConfig& cfg, const GameGrid& grid, int score); 
}