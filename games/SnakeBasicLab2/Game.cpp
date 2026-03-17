// Game.cpp
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include "Vec2.h"
#include "MenuInput.h"
#include "GameInput.h"

#include <cstdlib>      // std::system
#include <iostream>
#include <iomanip>      // format printed output, formatting tools for streams. 
#include <sstream>      // string streams. A stream that works on strings
#include <algorithm>
#include <chrono>
#include <thread>
#include <Windows.h>



// namespace or static (outside classes only!) on free functions does kind of the same thing? 
// making it an internal to this script only / internal linkage / invisible to outside scripts kind of function 
namespace
{

    // game-over box draver helpers 
    constexpr int kInnerWidth = 29;

    std::string BoxTop()
    {
        return " " + std::string(kInnerWidth + 2, '_');
    }

    std::string BoxLine(const std::string& text)
    {
        std::ostringstream out;
        out << "| " << std::left << std::setw(kInnerWidth) << text << "|";
        return out.str();
    }

    std::string BoxBottom()
    {
        return "|" + std::string(kInnerWidth + 1, '_') + "|";
    }

    // ----------------------------------------------------------------------

    void SetCursorHome()
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        std::cout.flush(); 

        COORD coord{ 0,0 };
        SetConsoleCursorPosition(hOut, coord);
    }

    void HideCursor()
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_CURSOR_INFO info{};
        GetConsoleCursorInfo(hOut, &info);
        info.bVisible = FALSE;
        SetConsoleCursorInfo(hOut, &info); 
    }

    void ShowCursor()
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_CURSOR_INFO info{};
        GetConsoleCursorInfo(hOut, &info);
        info.bVisible = TRUE;
        SetConsoleCursorInfo(hOut, &info); 
    }

}




Game::Game()
    : m_cfg{}
    , m_grid{m_cfg}
    , m_rootSeed(m_cfg.seed)
    , m_rngFruit()
{
    std::seed_seq fruitSeed
    {
        m_rootSeed,
        0xF17E1234u     // use of fixed valid constant/tag for the fruit subsystem. What is valid constants? 
    };

    m_rngFruit.seed(fruitSeed);
}


// ----- Small Game helpers / math -----

void Game::ClampPosition(Vec2& pos)
{

    if (pos.x < 0) pos.x = 0;
    if (pos.x >= m_grid.Width()) pos.x = m_grid.Width() - 1;
    if (pos.y < 0) pos.y = 0;
    if (pos.y >= m_grid.Height()) pos.y = m_grid.Height() - 1;
}

void Game::WrapPosition(Vec2& pos)
{

    if (pos.x < 0) pos.x = m_grid.Width() - 1;
    if (pos.x >= m_grid.Width()) pos.x = 0;
    if (pos.y < 0) pos.y = m_grid.Height() - 1;
    if (pos.y >= m_grid.Height()) pos.y = 0;
}


// ----- Render Methods -----

void Game::PressEnterPrompt()
{
    std::cout << "\nPress ENTER to return to menu...";
}

void Game::RenderMainMenu()
{
    std::system("cls");
    std::cout << "\n=== SNAKE MAIN MENU ===\n";
    std::cout << "1) Play" << std::endl;
    std::cout << "2) Controls / Help\n";
    std::cout << "3) Leaderboards\n";
    std::cout << "4) Quit\n";
    std::cout << "Select: ";
}

void Game::RenderHelpMenu()
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

void Game::RenderScoreBoard()
{
    std::system("cls");
    std::cout << "\n--- Leaderboard ---\n";
    std::cout << "\nBest score: " << m_highScore << "\n";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}

// placeholder 
void Game::RenderGameOver()
{
    std::system("cls");

    std::cout << BoxTop() << '\n';
    std::cout << BoxLine("") << '\n';
    std::cout << BoxLine("--- Game Over ---") << '\n';
    std::cout << BoxLine("") << '\n';
    std::cout << BoxLine("High Score: " + std::to_string(m_highScore)) << '\n';
    std::cout << BoxLine("Score:      " + std::to_string(m_score)) << '\n';
    std::cout << BoxLine("") << '\n';
    std::cout << BoxLine("Enter your name: _ _ _") << '\n';
    std::cout << BoxBottom() << '\n';

    // For now since it does not save anything yet
    std::string input;
    std::getline(std::cin, input);
}

void Game::RenderPlaying()
{
    SetCursorHome(); 

    std::cout << "WASD = Move    Q = Menu\n\n";

    std::cout << "Score: " << m_score << "\n\n";
    m_grid.ClearGrid();
    PaintFood();
    PaintSnake();
    RenderGrid(m_cfg, m_grid);

    std::cout.flush(); 
}




// ----- Gameplay -----

void Game::PaintSnake()
{
    bool isHead = true;

    for (const Vec2& coord : m_snake.Body())
    {
        if (!m_grid.InBounds(coord))
            continue;

        m_grid.SetCell(coord, isHead ? m_cfg.snakeHead : m_cfg.snakeBody);
        isHead = false;
    }
}

void Game::PaintFood()
{
    if (!m_food.HasActiveFruit()) return; 

    m_grid.SetCell(m_food.GetFruitCoord(), m_cfg.apple);
}



bool Game::IsCellBlockedForFood(const Vec2& pos)
{
    // return true if any blockers exist 
    if (!m_grid.InBounds(pos)) return true; 
    if (m_snake.Occupies(pos)) return true; 

    // add future checks, like: 
    // - AnyFoodOccupies(pos) if more than one food can spwan later. 
    // - WallOccupies(pos) for walls in the grid - maze levels, etc. 

    return false;
}

bool Game::TrySpawnFoodAtEmpty(const Vec2& pos)
{
    //  this would have worked, only in this specific order since GetCell returns m_empty if out-of-bounds
    if (IsCellBlockedForFood(pos)) return false;

    if (m_food.HasActiveFruit()) 
        ClearFruit();

    m_food.SetFruitStatus(true, pos);
    return true; 
}

bool Game::TrySpawnFoodAtRandom()
{
    const int width = m_grid.Width();
    const int height = m_grid.Height(); 

    // find a way to get random result
    std::uniform_int_distribution<int> xDist(0, width - 1);         // function object, used here to generate a random int in the set bounds
    std::uniform_int_distribution<int> yDist(0, height - 1);

    const int maxTries = std::max(10, (width * height) / 4);        // going with fewer tries to make a hybrid: first do a few "true random" tries, then fallback to scan empty cells

    // random tries first
    for (int i = 0; i < maxTries; ++i)
    {
        const int x = xDist(m_rngFruit);
        const int y = yDist(m_rngFruit);
        const Vec2 pos{ x, y };

        if(TrySpawnFoodAtEmpty(pos))
            return true;
    }

    // if the first rng placer fails change to scan for empty to make a list and rng choose one of those.
    std::vector<Vec2> emptyCells;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Vec2 pos{ x, y };                 // remember to put const on values that will not change after creation

            if (!IsCellBlockedForFood(pos))
                emptyCells.push_back(pos); 
        }
    }

    if (emptyCells.empty()) 
        return false;

    const std::size_t lastIndex = emptyCells.size() - 1;
    std::uniform_int_distribution<std::size_t> pickDist(0, lastIndex);  // set up the search range we can pick from

    const std::size_t pickIndex = pickDist(m_rngFruit);                 // pick a random plase on the vector from 0-lastIndex
    const Vec2 chosenPos = emptyCells[pickIndex];                       // take the rng choosen place to put fruit on

    // call PlaceFruitAt() with this random pos that was generated
    return TrySpawnFoodAtEmpty(chosenPos);
}

void Game::ClearFruit()
{
    if (!m_food.HasActiveFruit()) return;

    m_food.SetFruitStatus(false);  // or should I make this: m_food.SetFruitStatus(false, Vec2{0,0}), or make the position null, or nothing in some other way
}


void Game::UpdateSnakeDirFromInput(const char cmd)
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

    m_snake.SetNextDirection(dir); 
}

bool Game::MoveSnake()
{
    if (m_snake.Empty() || m_snake.NextDir() == Direction::STOP)
        return false;

    Vec2 nextPos = m_snake.PeekNextMove();        
    if (!m_grid.InBounds(nextPos))
    {
        m_playerDied = true; 
        m_playing = false; 
        return false; 
    }

    const bool ateFood = m_food.HasActiveFruit() && nextPos == m_food.GetFruitCoord();
    m_snake.Move(ateFood); 

    if (m_snake.HitSelf())
    {
        m_playerDied = true; 
        m_playing = false;
        return false;
    }

    return ateFood; 
}


void Game::HandlePlayingInput()
{
    char cmd{}; 

    if (!GameInput::TryReadLatestNormalizedChar(cmd))
        return; 

    if (cmd == 'q')
    {
        m_playerDied = false; 
        m_playing = false;
        return;
    }

    // maybe put in a if(cmd == any of W/A/S/D then call UpdateSnake)
    UpdateSnakeDirFromInput(cmd);
}

void Game::UpdatePlaying()
{
    const bool ateFood = MoveSnake();

    if (!m_playing) return;

    if (ateFood)
    {
        ++m_score; 
        m_food.SetFruitStatus(false); 
        TrySpawnFoodAtRandom(); 

        m_cfg.tickSpeedMS = std::max(m_cfg.minTickSpeed, m_cfg.tickSpeedMS - m_cfg.speedStepMs);
    }
}


void Game::StartNewRun()
{
    const Vec2 startPos{ ((m_cfg.width / 2) - 1),((m_cfg.height / 2) - 1) };
    
    m_snake.ResetTo(startPos);
    m_score = 0;
    m_playerDied = false; 
    m_playing = true;

    // change to allow tickSpeedMS to be set by player, startTickMs will still be default fallback  
    m_cfg.tickSpeedMS = m_cfg.startTickMs; 

    TrySpawnFoodAtRandom(); 

    std::system("cls");
    HideCursor(); 
}

void Game::RunPlayLoop()
{
    using clock = std::chrono::steady_clock;
    auto lastTick = clock::now(); 

    RenderPlaying();

    while (m_playing)
    {
        if (m_state == GameState::Paused)
        {
            // HandlePausedInput();     // pause menu + game instructions(Move (W/A/S/D), menu (Q):) and optin to quit or return to main-menu here
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        HandlePlayingInput();

        const auto now = clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        if (elapsed.count() >= m_cfg.tickSpeedMS) 
        {
            UpdatePlaying();
            RenderPlaying(); 
            lastTick = now; 
        }

        // update score at each succefull movement in next step / when fruit exists change to that 


        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    ShowCursor();

    if (m_playerDied)
        RenderGameOver(); 
}



void Game::Run()
{
    char choice{};
    m_running = true;

    while (m_running)
    {

        switch (m_state)
        {
        case GameState::MainMenu:
        {
            RenderMainMenu();

            std::string choiceStr;
            std::getline(std::cin, choiceStr);

            std::string_view stringView = MenuInput::TrimLeftWS(choiceStr);
            if (!MenuInput::ValidateCharOrPrintCustomMsg(stringView, "1234", "Please enter a valid value number between 1-4\n"))
                continue;

            choice = stringView.front();

            if (choice == '1') m_state = GameState::Playing;
            else if (choice == '2' || choice == '3') m_state = GameState::SubMenu;
            else if (choice == '4') m_state = GameState::Quit;

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
                RenderScoreBoard();
            }

            m_state = GameState::MainMenu;
            break;
        }
        case GameState::Playing:
        {
            StartNewRun();
            RunPlayLoop();

            std::cout << "Score: " << m_score << std::endl;
            if (m_score > m_highScore)
                m_highScore = m_score;

            m_state = GameState::MainMenu;

            break;
        }
        case GameState::Paused:
        {

            break;
        }
        case GameState::Quit:
        {
            m_running = false;

            break;
        }
        }
    }
}