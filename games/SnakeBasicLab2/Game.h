// Game.h
#pragma once
#include "Vec2.h"

struct GameGrid; 
struct Snake; 


struct GameConfig
{
    // game settings 
    int tickSpeed{};

    // grid settings
    int width{ 16 };
    int height{ 12 };
    char empty{ '.' };
    char gridBoarder{ '#' };

    // icons
    char snakeHead{ 'O' };
    char snakeBody{ 'o' };
    char apple{ '?' };
};


enum GameState
{
    MainMenu,
    SubMenu,
    Playing,
    Paused,
    Quit,
};



// Game helpers / math
void ClampPosition(const GameGrid& grid, Vec2& pos);
void WrapPosition(const GameGrid& grid, Vec2& pos);

// Rendering
void PressEnterPrompt();
void RenderMainMenu();
void RenderHelpMenu();
void RenderScoreBoard(const int& bestScore);

// Gameplay
void PaintSnake(GameGrid& grid, const GameConfig& cfg, const Snake& snake);
void UpdateSnakeDirFromInput(Snake& snake, const char& cmd);
void StepSnake(const GameGrid& grid, Snake& snake, bool growThisTurn = false);
void RunPlayLoop(const GameConfig& cfg, Snake& snake, int& score);

void Run(); 