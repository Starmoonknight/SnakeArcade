// GameGrid.h
#pragma once
#include "Vec2.h" 
#include <vector>

struct GameConfig;

struct GameGrid
{
    int width{};
    int height{};
    char empty{};
    Vec2 fruitCoord{};
    std::vector<char> grid;

    GameGrid() = default;
    explicit GameGrid(const GameConfig& cfg);

    int Index(int x, int y) const;
    bool InBounds(const Vec2& pos) const;

    void SetCell(int x, int y, char c);
    char GetCell(int x, int y) const;

    void ClearGrid(); 
};



void RenderGrid(const GameGrid& grid, char gridBoarder);