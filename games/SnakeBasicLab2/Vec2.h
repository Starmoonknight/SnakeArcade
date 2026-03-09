#pragma once



struct Vec2
{
    int x{};
    int y{};
};

// allow comparison checks for Vec2, just like if( int a == int b )
inline bool operator==(const Vec2& a, const Vec2& b)
{
    return a.x == b.x && a.y == b.y;
}

// allow comparison checks for Vec2, just like if( int a != int b )
inline bool operator!=(const Vec2& a, const Vec2& b)
{
    return !(a == b);
}

// allow the value of a Vec2 to be added to another Vec2, just like an 'int a += int b'
inline Vec2& operator+=(Vec2& a, const Vec2& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

// create a new Vec2 from two provided Vec2's, just like an 'int a + int b' 
[[nodiscard]] inline Vec2 operator+(Vec2 a, const Vec2& b)
{
    // a is a copy of the left operand, modify the copy with += and then return the copy    (return a new value without changing the originals.)
    a += b;
    return a;
}

// allow the value of a Vec2 to be removed from another Vec2, just like an 'int a -= int b'
inline Vec2& operator-=(Vec2& a, const Vec2& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

// create a new Vec2 from two provided Vec2's, just like an 'int a - int b' 
[[nodiscard]] inline Vec2 operator-(Vec2 a, const Vec2& b)                      // [[nodiscard]] since 'Vec2 a' is a copy that is modified and then returnerd as a new value instead  of modifieng a ref directly  
{
    a -= b;                                                                     // -= modifies and returns 'Vec2 a', this works becase 'Vec2 a' is a new copy made here
    return a;
}

