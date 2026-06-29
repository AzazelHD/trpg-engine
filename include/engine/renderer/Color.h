#pragma once
#include <cstdint>

// Color/FColor are engine-side color types so renderer-facing code never needs
// to include SDL3/SDL.h just to pass a color around.
//
// - Color  -> 0-255 integer RGBA channels (maps 1:1 to SDL_Color).
// - FColor -> 0.0-1.0 float RGBA channels (maps 1:1 to SDL_FColor, used for
//             vertex colors in drawGeometry/fillTriangle).
//
// engine::Renderer converts Color/FColor to SDL_Color/SDL_FColor internally;
// no other code should need to do that conversion.
//
// [x] Define Color{r,g,b,a} (uint8_t, default = opaque white), 3-arg ctor
//       defaults alpha to 255.
// [x] Define FColor{r,g,b,a} (float, default = opaque white), 3-arg ctor
//       defaults alpha to 1.0f.
// [x] Add common constants: black(), white(), transparent().

struct Color
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;

    constexpr Color() = default;
    constexpr Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    static constexpr Color black() { return Color{0, 0, 0, 255}; }
    static constexpr Color white() { return Color{255, 255, 255, 255}; }
    static constexpr Color transparent() { return Color{0, 0, 0, 0}; }
};

struct FColor
{
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    constexpr FColor() = default;
    constexpr FColor(float r_, float g_, float b_, float a_ = 1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}

    static constexpr FColor black() { return FColor{0.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr FColor white() { return FColor{1.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr FColor transparent() { return FColor{0.0f, 0.0f, 0.0f, 0.0f}; }

    // Implicit conversion from integer Color (0-255 -> 0.0-1.0)
    FColor(const Color &c)
        : r(c.r / 255.0f), g(c.g / 255.0f), b(c.b / 255.0f), a(c.a / 255.0f)
    {
    }
};