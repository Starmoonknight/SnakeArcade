
#include "Snake.h"

struct Snake
{
    std::deque<Vec2> segments{};
    Direction dir = Direction::Up;

    bool Empty() const { return segments.empty(); }
    std::size_t Leangth() const { return segments.size(); }

    const Vec2& Head() const { return segments.front(); }
    const Vec2& Tail() const { return segments.back(); }

    void ResetTo(const Vec2& startPos, Direction startDir = Direction::Up)
    {
        segments.clear();
        segments.push_front(startPos);
        dir = startDir;
    }

    void AddHead(const Vec2& p) { segments.push_front(p); }
    void RemoveTail() { segments.pop_back(); }
};