// GameGrid.h
#pragma once
#include "Vec2.h" 
#include <vector>
#include <string>
#include <cstddef>      // std::size_t

struct GameConfig;

class GameGrid
{
public:
    // constructor(s)
    explicit GameGrid(const GameConfig& cfg);

    // public queries (read-only info)
    int Width() const;
    int Height() const;
    std::size_t CellCount() const; 
    bool InBounds(const Vec2& pos) const;
    bool IsEmptyCell(const Vec2& pos) const;

    char GetCell(const Vec2& pos) const;
    std::vector<std::string> ToLines(const GameConfig& cfg) const;
    std::string ToString(const GameConfig& cfg) const;

    // public actions / state changes
    void SetCell(const Vec2& pos, char c);
    void ClearGrid(); 


private:
    int m_width{};
    int m_height{};
    char m_empty{};
    std::vector<char> m_grid;

    std::size_t ToCellIndex(const Vec2& pos) const;
    Vec2 IndexToCellCoord(std::size_t index) const; 
};



void RenderGrid(const GameConfig& cfg, const GameGrid& grid);