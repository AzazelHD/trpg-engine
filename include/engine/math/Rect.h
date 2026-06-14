#pragma once
#include "Vec2.h"

// Pure math rectangle (engine layer).
// No SDL dependency here — keeps the engine portable and clean.

template <typename T>
struct Rect
{
    T x{}, y{}, w{}, h{};

    Rect() = default;

    Rect(T x, T y, T w, T h)
        : x(x), y(y), w(w), h(h) {}

    // [x]: Point inside rectangle
    bool contains(const Vec2<T> &point) const
    {
        return point.x >= x && point.x < x + w &&
               point.y >= y && point.y < y + h;
    }

    // [x]: AABB intersection
    bool intersects(const Rect<T> &other) const
    {
        return x < other.x + other.w &&
               x + w > other.x &&
               y < other.y + other.h &&
               y + h > other.y;
    }

    T right() const { return x + w; }
    T bottom() const { return y + h; }
};

using Recti = Rect<int>;
using Rectf = Rect<float>;
