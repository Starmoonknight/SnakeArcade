// Game.cpp
#include "Game.h"
#include "GameGrid.h"
#include "Input.h"
#include "Snake.h"
#include <cstdlib>      // std::system
#include <iostream>




// ----- Game helpers / math -----

void ClampPosition(const GameGrid& grid, Vec2& pos)
{

    if (pos.x < 0) pos.x = 0;
    if (pos.x >= grid.width) pos.x = grid.width - 1;
    if (pos.y < 0) pos.y = 0;
    if (pos.y >= grid.height) pos.y = grid.height - 1;
}

void WrapPosition(const GameGrid& grid, Vec2& pos)
{

    if (pos.x < 0) pos.x = grid.width - 1;
    if (pos.x >= grid.width) pos.x = 0;
    if (pos.y < 0) pos.y = grid.height - 1;
    if (pos.y >= grid.height) pos.y = 0;
}


// ----- Render Methods -----

void PressEnterPrompt()
{
    std::cout << "\nPress ENTER to return to menu...";
}

void RenderMainMenu()
{
    std::system("cls");
    std::cout << "\n=== SNAKE MAIN MENU ===\n";
    std::cout << "1) Play" << std::endl;
    std::cout << "2) Controls / Help\n";
    std::cout << "3) Leaderboards\n";
    std::cout << "4) Quit\n";
    std::cout << "Select: ";
}

void RenderHelpMenu()
{
    std::system("cls");
    std::cout << "\n--- Controls / Help ---\n";
    std::cout << "Player move with w/a/s/d\n ";
    std::cout << "In game press q to return to menu\n ";
    std::cout << "(More to come soon...)\n ";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}

void RenderScoreBoard(const int& bestScore)
{
    std::system("cls");
    std::cout << "\n--- Leaderboard ---\n";
    std::cout << "\nBest score: " << bestScore << "\n";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}




// ----- Gameplay -----

void PaintSnake(GameGrid& grid, const GameConfig& cfg, const Snake& snake)
{
    bool isHead = true;

    // PLACEHOLDER: painting all segments as the head char for now
    for (const Vec2& coord : snake.Body())
    {
        if (!grid.InBounds(coord))
            continue;

        grid.SetCell(coord.x, coord.y, isHead ? cfg.snakeHead : cfg.snakeBody);
        isHead = false;
    }
}

void UpdateSnakeDirFromInput(Snake& snake, const char& cmd)
{
    Direction dir{};

    switch (cmd)
    {
    case 'w': dir = Direction::Up;    break;
    case 's': dir = Direction::Down;  break;
    case 'a': dir = Direction::Left;  break;
    case 'd': dir = Direction::Right; break;
    default:  return;
    }

    snake.SetNextDirection(dir); 
}

void StepSnake(const GameGrid& grid, Snake& snake, bool growThisTurn)
{
    if (snake.Empty() || snake.NextDir() == Direction::STOP)
        return;

    Vec2 nextPos = snake.PeekNextMove();        
    if (!grid.InBounds(nextPos)) return;        // change to GameOver in next step 

    snake.Move(false); 

    // Also add body hit check at next step
    // Also add fruit hit check at next step

}

void RunPlayLoop(const GameConfig& cfg, Snake& snake, int& score)
{
    GameGrid grid{ cfg };

    const Vec2 startPos{ ((cfg.width / 2) - 1),((cfg.height / 2) - 1) };
    snake.ResetTo(startPos);

    bool playing = true;
    while (playing)
    {
        // RENDER
        grid.ClearGrid();
        PaintSnake(grid, cfg, snake);
        RenderGrid(grid, cfg.gridBoarder);

        // READ
        char cmd = ReadCharChoice("Move (W/A/S/D), menu (Q): ", "wasdq");

        // UPDATE 
        if (cmd == 'q')
        {
            playing = false;
            continue;
        }

        UpdateSnakeDirFromInput(snake, cmd);
        StepSnake(grid, snake /* growThisTurn = false */);
        // update score at each succefull movement in next step / when fruit exists change to that 
    }
}


void Run() 
{

    GameConfig cfg;
    GameState state = GameState::MainMenu;
    //GameGrid grid(cfg);
    Snake snake{};

    char choice{};
    int bestScore{};
    bool running{ true };

    while (running)
    {

        switch (state)
        {
        case GameState::MainMenu:
        {
            RenderMainMenu();

            std::string choiceStr;
            std::getline(std::cin, choiceStr);

            std::string_view stringView = TrimLeftWS(choiceStr);
            if (!ValidateCharOrPrintCustomMsg(stringView, "1234", "Please enter a valid value number between 1-4\n"))
                continue;

            choice = stringView.front();

            if (choice == '1') state = GameState::Playing;
            else if (choice == '2' || choice == '3') state = GameState::SubMenu;
            else if (choice == '4') state = GameState::Quit;

            break;
        }
        case GameState::SubMenu:
        {
            if (choice == '2')
            {
                RenderHelpMenu();
            }
            else if (choice == '3')
            {
                RenderScoreBoard(bestScore);
            }

            state = GameState::MainMenu;
            break;
        }
        case GameState::Playing:
        {
            int score{ 0 };
            RunPlayLoop(cfg, snake, score);

            std::cout << "Score: " << score << std::endl;
            if (score > bestScore)
                bestScore = score;

            state = GameState::MainMenu;

            break;
        }
        case GameState::Paused:
        {

            break;
        }
        case GameState::Quit:
        {
            running = false;

            break;
        }
        }
    }
}