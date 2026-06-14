#pragma once

#include <string>

// Vec2i — integer 2D vector, used for tile/grid coordinates.
// Vec2f — float 2D vector, used for world/screen coordinates.
//
// [x]: Implement both as simple structs with:
//   - x, y members
//   - operator+, operator-, operator* (scalar), operator==, operator!=
//   - A to-string method (useful for logging/debugging)
//
// Keep them as structs (not classes) — public members are fine here, they're plain data.
//
// Tip: Vec2i and Vec2f are almost identical. Consider a template Vec2<T> and then
//      using Vec2i = Vec2<int>; and Vec2f = Vec2<float>;
//      This avoids duplicating the operator code.

template <typename T>
struct Vec2
{
    T x{}, y{};

    Vec2<T> operator+(const Vec2<T> &other) const
    {
        return {x + other.x, y + other.y};
    }

    Vec2<T> &operator+=(const Vec2<T> &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2<T> operator-(const Vec2<T> &other) const
    {
        return {x - other.x, y - other.y};
    }

    Vec2<T> &operator-=(const Vec2<T> &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2<T> operator*(T scalar) const
    {
        return {x * scalar, y * scalar};
    }

    Vec2<T> operator/(T scalar) const
    {
        return {x / scalar, y / scalar};
    }

    bool operator==(const Vec2<T> &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2<T> &other) const
    {
        return !(*this == other);
    }

    std::string toString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
