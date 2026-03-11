// GameGrid.h
#pragma once
#include "Vec2.h" 
#include <vector>
#include <string>

struct GameConfig;

class GameGrid
{
public:
    // constructor(s)
    GameGrid() = default;
    explicit GameGrid(const GameConfig& cfg);

    // public queries (read-only info)
    int Width() const;
    int Height() const;

    int ToIndex(const Vec2& pos) const;
    bool InBounds(const Vec2& pos) const;

    char GetCell(const Vec2& pos) const;
    bool HasFruit() const;
    Vec2 GetFruitCoord() const; 

    std::string ToString(char gridBorderH, char gridBorderV) const;

    // public actions / state changes
    void SetCell(const Vec2& pos, char c);
    void PlaceFruitAt(const Vec2& pos, char c);
    void PlaceFruitRandom(char c);

    void ClearFruit(); 
    void ClearGrid(); 


private:
    int m_width{};
    int m_height{};
    char m_empty{};
    bool m_hasFruit{ false };
    Vec2 m_fruitCoord{};
    std::vector<char> m_grid;
};



void RenderGrid(const GameGrid& grid, char gridBorderH, char gridBorderV);