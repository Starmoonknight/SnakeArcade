// Snake.cpp
#include "Snake.h"


static Vec2 Delta(Direction dir)
{
    switch (dir)
    {
    case Direction::STOP:  break;
    case Direction::Up:    return { 0, -1 };
    case Direction::Down:  return { 0,  1 };
    case Direction::Left:  return { -1, 0 };
    case Direction::Right: return { 1,  0 };
    }
    return { 0,0 }; // fallback
}

static bool Reversing(Direction currentDir, Direction nextDir)
{
    const bool reversing =
        (currentDir == Direction::Up && nextDir == Direction::Down) ||
        (currentDir == Direction::Down && nextDir == Direction::Up) ||
        (currentDir == Direction::Left && nextDir == Direction::Right) ||
        (currentDir == Direction::Right && nextDir == Direction::Left);

    return reversing; 
}



// Public from Snake

bool Snake::Empty() const { return m_body.empty(); }
std::size_t Snake::Length() const { return m_body.size(); }
const std::deque<Vec2>& Snake::Body() const { return m_body; }

Vec2 Snake::Head() const { return Empty() ? Vec2{} : m_body.front(); }
Vec2 Snake::Tail() const { return Empty() ? Vec2{} : m_body.back(); }

Direction Snake::Dir() const { return m_dir; }
Direction Snake::NextDir() const { return m_nextDir; }


void Snake::ResetTo(const Vec2& startPos, int startLength, Direction startDir)
{
    if (startLength < 1) startLength = 1; 

    m_body.clear();
    m_body.push_front(startPos);
    m_dir = startDir;
}

void Snake::SetNextDirection(Direction dir)
{
    if (!Reversing(m_dir, dir))
        m_nextDir = dir; 
}

void Snake::Move(bool grow)
{
    if (m_nextDir == Direction::STOP) return; // can use STOP in the m_nextDir while not ruining the saved dir

    const Vec2 nextHeadPos = PeekNextMove();
    m_dir = m_nextDir;

    AddHead(nextHeadPos);
    if (!grow)
        RemoveTail(); 
}

Vec2 Snake::PeekNextMove() const
{
    const Vec2 head = Head();
    const Vec2 newHeadPos = head + Delta(m_nextDir);

    return newHeadPos; 
}

bool Snake::Occupies(const Vec2& pos) const
{
    for (const Vec2& bodySegment : m_body)
    {
        if (bodySegment == pos) return true; 
    }
    return false; 
}

bool Snake::HitSelf() const
{
    if (Length() < 4) return false;

    const Vec2 head = Head(); 
    bool first = true; 
    for (const Vec2& bodySegment : m_body)
    {
        if (first) { first = false; continue; }
        if (bodySegment == head) return true; 
    }
    return false; 
}



// Private from Snake

void Snake::AddHead(const Vec2& p) { m_body.push_front(p); }

void Snake::RemoveTail() { m_body.pop_back(); }




