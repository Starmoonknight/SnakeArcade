// GameGrid.cpp
#include "GameGrid.h"
#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cstddef>



GameGrid::GameGrid(const GameConfig& cfg)
    : width(cfg.width)
    , height(cfg.height)
    , empty(cfg.empty)
    , grid(static_cast<std::size_t>(cfg.width)* static_cast<std::size_t>(cfg.height), cfg.empty)
{
}

// converts (x,y) to a 1D index.
int GameGrid::Index(int x, int y) const
{
    int coord = y * width + x;
    return coord;
}

// checks against current board limits
bool GameGrid::InBounds(const Vec2& pos) const
{
    return (pos.x >= 0 && pos.x < width)
        && (pos.y >= 0 && pos.y < height);
}

// writes a char to a grid cell
void GameGrid::SetCell(int x, int y, char c)
{
    grid[static_cast<std::size_t>(Index(x, y))] = c;
}

// returns char at (x,y), assumes caller gives valid coords and has nos no checks. 
// returns same as board.grid[board.Index(x, y)] would have done 
char GameGrid::GetCell(int x, int y) const
{
    return grid[static_cast<std::size_t>(Index(x, y))];
}

void GameGrid::ClearGrid()
{
    std::fill(grid.begin(), grid.end(), empty);
}



void RenderGrid(const GameGrid& grid, char gridBoarder)       // char gridBoarder, why not const + &   Answer: char is small, a ref pointer is not less expensive so not worth it.      (does a char_view exist?)  
{
    for (int x = 0; x < grid.width + 2; ++x)   // top row rim
        std::cout << gridBoarder << ' ';

    std::cout << "\n";

    for (int y = 0; y < grid.height; ++y)
    {
        std::cout << gridBoarder << ' ';            // left row rim
        for (int x = 0; x < grid.width; ++x)
        {
            std::cout << grid.GetCell(x, y) << ' ';
        }
        std::cout << gridBoarder << ' ';            // right row rim 
        std::cout << '\n';
    }

    for (int x = 0; x < grid.width + 2; ++x)   // bottom row rim
        std::cout << gridBoarder << ' ';

    std::cout << '\n';
}
