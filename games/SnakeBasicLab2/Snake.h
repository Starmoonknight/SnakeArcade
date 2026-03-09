#pragma once
#include <deque>
#include <cstddef>
#include "Vec2.h"
#include "Direction.h"


struct Snake
{
public:
    void ResetTo(const Vec2& startPos, Direction startDir = Direction::Up);

    const Vec2& Head() const;
    const Vec2& Tail() const;
    bool Empty() const;
    std::size_t Leangth() const;


    void AddHead(const Vec2& p) { m_segments.push_front(p); }
    void RemoveTail() { m_segments.pop_back(); }

    void SetDirection(Direction dir);
    Vec2 NextHead();

    void Move(bool grow);
    bool Occupies(const Vec2& pos) const;
    bool HitSelf() const; 


private:
    std::deque<Vec2> m_segments{};
    Direction m_dir{ Direction::Up };
};