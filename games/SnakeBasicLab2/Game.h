// Game.h
#pragma once
#include "GameGrid.h"
#include "Snake.h"
#include "Food.h"
#include <cstdint>      // int in size instead of long, like: uint32_t
#include <random>

struct Vec2;


struct GameConfig
{
    // game settings 
    int tickSpeed{};

    // grid settings
    int width{ 16 };
    int height{ 12 };
    char empty{ '.' };
    char gridBorderH{ '#' };
    char gridBorderV{ '#' };

    // icons
    char snakeHead{ 'O' };
    char snakeBody{ 'o' };
    char apple{ '?' };

    // rng
    std::uint32_t seed{ 12345u };
};


enum GameState
{
    MainMenu,
    SubMenu,
    Playing,
    Paused,
    Quit,
};


/*

// Looked at an idea of making a data package of the result to handle combinatins more easily.
// Wayyyyyyyy overscoped for a snake game but intresting to look at for something where resolve order matters a lot and has more moving parts

struct MoveResult
{
    bool moved{ false };
    bool ateFruit{ false };
    bool hitWall{ false };
    bool hitSelf{ false };
    bool atePowerUp{ false };
    bool ateEnemy{ false };
};


// Using these two enums are also overscoped for snake but wanted to try one of my ideas, later, for handling game state updates
enum MoveResult
{
    NoMove,
    Moved,
    HitWall,
    HitSelf,
};

enum SpecialEffects
{
    None,
    AteFruit,
};
*/



class Game
{
public:
    Game(); 
    void Run(); 

private:
    GameConfig m_cfg;
    GameGrid m_grid;
    Snake m_snake;
    Food m_food; 

    std::uint32_t m_rootSeed{};
    std::mt19937 m_rngFruit;

    int m_score{ 0 };
    int m_highScore{ 0 };
    bool m_running{ false };
    bool m_playing{ false };
    GameState m_state{ GameState::MainMenu }; 

private:
    // Small Game logic helpers / math
    void ClampPosition(Vec2& pos);
    void WrapPosition(Vec2& pos);

    // Rendering
    void PressEnterPrompt();
    void RenderMainMenu();
    void RenderHelpMenu();
    void RenderScoreBoard();
    void RenderGameOver(); 

    // Render Helper
    void PaintSnake();
    void PaintFood(); 

    // Gameplay Helpers
    bool IsCellBlockedForFood(const Vec2& pos);
    bool TrySpawnFoodAtEmpty(const Vec2& pos); 
    bool TrySpawnFoodAtRandom(); 
    void ClearFruit();

    // Gameplay
    void UpdateSnakeDirFromInput(const char cmd);
    bool MoveSnake();

    void HandlePlayingInput();
    void UpdatePlaying();
    void RenderPlaying();

    void RunPlayLoop();
};

