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
    , m_rootSeed(cfg.seed)
    , m_rngFruit()
{
    std::seed_seq fruitSeed
    {
        m_rootSeed,
        0xF17E1234u     // use of fixed valid constant/tag for the fruit subsystem. What is valid constants? 
    }; 

    m_rngFruit.seed(fruitSeed); 
}

int GameGrid::Width() const { return m_width;  }
int GameGrid::Height() const { return m_height; }


// checks against current board limits
bool GameGrid::InBounds(const Vec2& pos) const
{
    return (pos.x >= 0 && pos.x < m_width)
        && (pos.y >= 0 && pos.y < m_height);
}

bool GameGrid::IsEmptyCell(const Vec2& pos) const
{
    return InBounds(pos) && m_grid[ToCellIndex(pos)] == m_empty;
}

// Returns the character at the given position.
// If the position is out of bounds, returns m_empty.
char GameGrid::GetCell(const Vec2& pos) const
{
    if (!InBounds(pos)) return m_empty; 
    return m_grid[ToCellIndex(pos)];
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
    s += '\n';      // use '' when one char, and "" with more text 

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
    m_grid[ToCellIndex(pos)] = c;
}

void GameGrid::PlaceFruitAtEmpty(const Vec2& pos, char c)
{
    //  this would have worked, only in this specific order since GetCell returns m_empty if out-of-bounds 
    //if (!InBounds(pos) || GetCell(pos) != m_empty) return;

    if (!InBounds(pos)) return;
    if (m_grid[ToCellIndex(pos)] != m_empty) return;

    if (m_hasFruit)
        ClearFruit(); 

    m_hasFruit = true; 
    m_fruitCoord = pos;
    SetCell(m_fruitCoord, c);
}

// a hybrid function to test out two rng models 
void GameGrid::PlaceFruitRandom(char c)
{
    // find a way to get random result
    std::uniform_int_distribution<int> xDist(0, m_width - 1);           // function object, used here to generate a random int in the set bounds 
    std::uniform_int_distribution<int> yDist(0, m_height - 1);

    const int maxTries = std::max(10, (m_width * m_height) / 4);        // going with fewer tries to make a hybrid: first do a few "true random" tries, then fallback to scan empty cells

    // random tries first
    for (int i = 0; i < maxTries; ++i)
    {
        const int x = xDist(m_rngFruit);
        const int y = yDist(m_rngFruit);
        const Vec2 pos{ x, y };
        
        if (IsEmptyCell(pos))
        {
            PlaceFruitAtEmpty(pos, c);
            return; 
        }
    }

    // if the first rng placer fails change to scan for empty to make a list and rng choose one of those. 
    std::vector<Vec2> emptyCells;
    
    for (std::size_t i = 0; i < m_grid.size(); ++i)
    { 
        if (m_grid[i] != m_empty) continue;

        const Vec2 pos = IndexToCellCoord(i);                           // remember to put const on values that will not change after creation
        emptyCells.push_back(pos); 
    }

    if (emptyCells.empty()) return; 
    
    const std::size_t lastIndex = emptyCells.size() - 1; 
    std::uniform_int_distribution<std::size_t> pickDist(0, lastIndex);  // set up the search range we can pick from

    const std::size_t pickIndex = pickDist(m_rngFruit);                 // pick a random plase on the vector from 0-lastIndex 
    const Vec2 chosenPos = emptyCells[pickIndex];                       // take the rng choosen place to put fruit on

    // call PlaceFruitAt() with this random pos that was generated 
    PlaceFruitAtEmpty(chosenPos, c);
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


// converts (x,y) to a 1D index.
std::size_t GameGrid::ToCellIndex(const Vec2& pos) const
{
    return static_cast<std::size_t>(pos.y * m_width + pos.x);
}

Vec2 GameGrid::IndexToCellCoord(std::size_t index) const
{
    int x = static_cast<int>(index % static_cast<std::size_t>(m_width));
    int y = static_cast<int>(index / static_cast<std::size_t>(m_width));

    return Vec2{ x, y };
}


