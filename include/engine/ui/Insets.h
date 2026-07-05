// engine/ui/Insets.h
#pragma once

struct Insets
{
    float top{};
    float right{};
    float bottom{};
    float left{};

    constexpr Insets() = default;

    constexpr explicit Insets(float all)
        : top(all), right(all), bottom(all), left(all)
    {
    }

    constexpr Insets(float horizontal, float vertical)
        : top(vertical),
          right(horizontal),
          bottom(vertical),
          left(horizontal)
    {
    }

    constexpr Insets(float top, float right, float bottom, float left)
        : top(top),
          right(right),
          bottom(bottom),
          left(left)
    {
    }

    static constexpr Insets all(float value)
    {
        return Insets(value);
    }

    static constexpr Insets symmetric(float horizontal, float vertical)
    {
        return Insets(horizontal, vertical);
    }
};