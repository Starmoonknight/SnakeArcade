// Snake.h
#pragma once
#include "Vec2.h"
#include "Direction.h"
#include <deque>
#include <cstddef>      // std::size_t


struct Snake
{
public:
    Snake() = default; 

    bool Empty() const;
    std::size_t Length() const;
    const std::deque<Vec2>& Body() const;
    
    Vec2 Head() const; 
    Vec2 Tail() const;
    
    Direction Dir() const;
    Direction NextDir() const;

    void ResetTo(const Vec2& startPos, int startLength = 1, Direction startDir = Direction::Up);
    void SetNextDirection(Direction dir);
    void Move(bool grow);

    Vec2 PeekNextMove() const; 
    bool Occupies(const Vec2& pos) const;
    bool HitSelf() const; 


private:
    std::deque<Vec2> m_body{};
    Direction m_dir{ Direction::Up };
    Direction m_nextDir{ Direction::Up };

    void AddHead(const Vec2& pos); 
    void RemoveTail();
};

