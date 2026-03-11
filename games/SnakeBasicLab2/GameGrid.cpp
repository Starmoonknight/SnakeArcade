// GameGrid.cpp
#include "GameGrid.h"
#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cstddef>



void RenderGrid(const GameGrid& grid, char gridBorderH, char gridBorderV)       // char gridBoarder, why not const + &   Answer: char is small, a ref pointer is not less expensive so not worth it.      (does a char_view exist?)  
{
    std::cout << grid.ToString(gridBorderH, gridBorderV);
}




GameGrid::GameGrid(const GameConfig& cfg)
    : m_width(cfg.width)
    , m_height(cfg.height)
    , m_empty(cfg.empty)

    // Later: add validation and do not use raw values to build grid. Ensure no negative values, etc. 
    , m_grid(static_cast<std::size_t>(cfg.width)* static_cast<std::size_t>(cfg.height), cfg.empty)
{
}

int GameGrid::Width() const { return m_width;  }
int GameGrid::Height() const { return m_height; }


// converts (x,y) to a 1D index.
int GameGrid::ToIndex(const Vec2& pos) const
{
    int coord = pos.y * m_width + pos.x;
    return coord;
}

// checks against current board limits
bool GameGrid::InBounds(const Vec2& pos) const
{
    return (pos.x >= 0 && pos.x < m_width)
        && (pos.y >= 0 && pos.y < m_height);
}

// Returns the character at the given position.
// If the position is out of bounds, returns m_empty.
char GameGrid::GetCell(const Vec2& pos) const
{
    if (!InBounds(pos)) return m_empty; 
    size_t idx = static_cast<std::size_t>(ToIndex(pos));
    return m_grid[idx];
}

bool GameGrid::HasFruit() const { return m_hasFruit; }

Vec2 GameGrid::GetFruitCoord() const { return m_fruitCoord; }

std::string GameGrid::ToString(char gridBorderH, char gridBorderV) const
{
    // found out about lambdas so trying one here
    const auto estimatedCapacity = [this]() -> std::size_t
    {
        return static_cast<std::size_t>((m_height + 2) * (2 * m_width + 5));
    };

    /*
    // probably best to use this if not a plain value
    const std::size_t estimatedCapacity =
        static_cast<std::size_t>((m_height + 2) * (2 * m_width + 5));
    
    s.reserve(estimatedCapacity);
    */

    // if using a plain value
    //s.reserve(200);     // why use set number insted of formula like: s.reserve(static_cast<size_t>((m_width * 2 + 1) * m_height + 64));

    std::string s;
    s.reserve(estimatedCapacity()); 

    for (int x = 0; x < m_width + 2; ++x)
    {
        s += gridBorderH;                   // top row rim
        s += ' ';
    }
    s += '\n';

    for (int y = 0; y < m_height; ++y)
    {
        s += gridBorderV;                   // left row rim
        s += ' ';    

        for (int x = 0; x < m_width; ++x)
        {
            Vec2 pos{ x, y };
            s += GetCell(pos);
            s += ' ';
        }

        s += gridBorderV;                   // right row rim
        s += ' ';        
        s += '\n';
    }

    for (int x = 0; x < m_width + 2; ++x)
    {
        s += gridBorderH;                   // bottom row rim
        s += ' ';
    }
    s += '\n';

    return s; 
}

// writes a char to a grid cell
void GameGrid::SetCell(const Vec2& pos, char c)
{
    if (!InBounds(pos)) return;

    const std::size_t idx = static_cast<std::size_t>(ToIndex(pos));
    m_grid[idx] = c;
}

void GameGrid::PlaceFruitAt(const Vec2& pos, char c)
{
    if (!InBounds(pos)) return;

    const std::size_t idx = static_cast<std::size_t>(ToIndex(pos));
    if (m_grid[idx] != m_empty) return;

    if (m_hasFruit)
        ClearFruit(); 

    m_hasFruit = true; 
    m_fruitCoord = pos;
    SetCell(m_fruitCoord, c);
}

void GameGrid::PlaceFruitRandom(char c)
{
    // find a way to get random result

    // call PlaceFruitAt() with this random pos that was generated 
}

void GameGrid::ClearFruit()
{
    if (!m_hasFruit) return; 

    m_hasFruit = false;
    SetCell(m_fruitCoord, m_empty); 
    m_fruitCoord = Vec2{ 0,0 };  // or should I make this null, or nothing in some other way
}

void GameGrid::ClearGrid()
{
    std::fill(m_grid.begin(), m_grid.end(), m_empty);
}


