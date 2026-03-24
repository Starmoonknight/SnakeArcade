// GameGrid.cpp
#include "GameGrid.h"
#include "Game.h"
#include <iostream>
#include <string_view>


namespace
{
    void AppendStyledCell(std::string& out, char c,
        std::string_view style, std::string_view reset)
    {
        out.append(style.data(), style.size());
        out.push_back(c);
        out.push_back(' ');
        out.append(reset.data(), reset.size()); 
    }

    std::string_view StyleForCell(char c, const GameConfig& cfg)
    {
        if (c == cfg.snakeHead) return cfg.snakeHeadStyle;
        if (c == cfg.snakeBody) return cfg.snakeBodyStyle;
        if (c == cfg.apple) return cfg.appleStyle;
        return cfg.emptyStyle; 
    }
}


void RenderGrid(const GameConfig& cfg, const GameGrid& grid)       // char gridBoarder, why not const + &   Answer: char is small, a ref pointer is not less expensive so not worth it.      (does a char_view exist?)  
{
    std::cout << grid.ToString(cfg);
}



GameGrid::GameGrid(const GameConfig& cfg)
    : m_width(cfg.width)
    , m_height(cfg.height)
    , m_empty(cfg.empty)

    // Later: add validation and do not use raw values to build grid. Ensure no negative values, etc. 
    , m_grid(static_cast<std::size_t>(cfg.width)* static_cast<std::size_t>(cfg.height), cfg.empty)
{ }

int GameGrid::Width() const { return m_width;  }
int GameGrid::Height() const { return m_height; }
std::size_t GameGrid::CellCount() const { return m_grid.size(); }


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

std::vector<std::string> GameGrid::ToLines(const GameConfig& cfg) const
{
    const char gridBorderH = cfg.gridBorderH;
    const char gridBorderV = cfg.gridBorderV;

    std::vector<std::string> lines;
    lines.reserve(m_height + 2); 

    std::string tempLine; 
    tempLine.reserve((m_width + 2) * 2); 

    // internal lambda to avoid repeat line, but specific enough to not warrant it's own function
    auto commitLine = [&]()
    {
        lines.push_back(tempLine);          // could also use lines.push_back(std::move(tempLine)); but unsure why? Need to look into copy vs move and then also emplace
        tempLine.clear();
    };

    // build the top grid-border row 
    for (int x = 0; x < m_width + 2; ++x)
    {
        tempLine += gridBorderH;                // top row rim
        tempLine += ' ';                        // use '' when one char, and "" with more text 
    }
    commitLine(); 

    // build the actuall grid rows
    for (int y = 0; y < m_height; ++y)
    {
        tempLine += gridBorderV;                // left row rim
        tempLine += ' ';

        for (int x = 0; x < m_width; ++x)
        {
            Vec2 pos{ x, y };
            tempLine += GetCell(pos);
            tempLine += ' ';
        }

        tempLine += gridBorderV;                // right row rim
        tempLine += ' ';

        commitLine();                           // add every line to the vector 
    }

    // build the bottom grid-border row 
    for (int x = 0; x < m_width + 2; ++x)
    {
        tempLine += gridBorderH;                // bottom row rim
        tempLine += ' ';
    }
    commitLine();

    return lines;
}

std::string GameGrid::ToString(const GameConfig& cfg) const
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

    std::string tempString;
    tempString.reserve(estimatedCapacity()); 

    std::vector<std::string> gridLines = ToLines(cfg); 
    for (const std::string& line : gridLines)
    {
        tempString += line;
        tempString += '\n'; 
    }

    return tempString; 
}

// switch to after lab 2
/*
std::string GameGrid::ToString(const GameConfig& cfg) const
{
    const auto estimatedCapacity = [this]() -> std::size_t
        {
            return static_cast<std::size_t>((m_height + 2) * (2 * m_width + 20));
        };

    std::string s;
    s.reserve(estimatedCapacity());

    for (int x = 0; x < m_width + 2; ++x)
    {
        AppendStyledCell(s, cfg.gridBorderH, cfg.borderStyle, cfg.resetStyle);
    }
    s += '\n';

    for (int y = 0; y < m_height; ++y)
    {
        AppendStyledCell(s, cfg.gridBorderV, cfg.borderStyle, cfg.resetStyle);

        for (int x = 0; x < m_width; ++x)
        {
            const Vec2 pos{ x, y };
            const char cell = GetCell(pos);
            AppendStyledCell(s, cell, StyleForCell(cell, cfg), cfg.resetStyle);
        }

        AppendStyledCell(s, cfg.gridBorderV, cfg.borderStyle, cfg.resetStyle);
        s += '\n';
    }

    for (int x = 0; x < m_width + 2; ++x)
    {
        AppendStyledCell(s, cfg.gridBorderH, cfg.borderStyle, cfg.resetStyle);
    }
    s += '\n';

    return s;
}
*/


// writes a char to a grid cell
void GameGrid::SetCell(const Vec2& pos, char c)
{
    if (!InBounds(pos)) return;
    m_grid[ToCellIndex(pos)] = c;
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


