#pragma once

#include "engine/math/MathUtils.h"

#include <algorithm>

// Tween<T> interpolates a single value over a fixed duration using an easing
// function from engine/math/MathUtils.h.
//
// T must support the expression:  from + (to - from) * t
// which covers arithmetic scalars and Vec2<T> (e.g. Vec2f tile/screen tweens).
// Tween is a plain driven value — it does not own a clock and it never calls
// into the renderer, so it is safe to use from update() on any thread the game
// already uses.
template <typename T>
class Tween
{
public:
    using EasingFunction = float (*)(float);

    // duration is clamped to a small positive value so update() can never
    // divide by zero; easing defaults to easeInOut from MathUtils.
    void start(const T &from, const T &to, float duration, EasingFunction easing = easeInOut)
    {
        m_from = from;
        m_to = to;
        m_duration = std::max(duration, 0.0001f);
        m_easing = easing ? easing : linear;
        m_elapsed = 0.0f;
        m_progress = 0.0f;
        m_active = true;
        m_finished = false;
        m_value = m_from;
    }

    void update(float dt)
    {
        if (!m_active)
            return;

        m_elapsed += std::max(dt, 0.0f);
        m_progress = std::min(m_elapsed / m_duration, 1.0f);
        m_value = m_from + (m_to - m_from) * m_easing(m_progress);

        if (m_progress >= 1.0f)
        {
            m_value = m_to;
            m_active = false;
            m_finished = true;
        }
    }

    void stop()
    {
        m_active = false;
        m_finished = true;
    }

    [[nodiscard]] const T &value() const { return m_value; }
    [[nodiscard]] float progress() const { return m_progress; }
    [[nodiscard]] bool isActive() const { return m_active; }
    [[nodiscard]] bool isFinished() const { return m_finished; }

private:
    T m_from{};
    T m_to{};
    T m_value{};
    EasingFunction m_easing = easeInOut;
    float m_duration = 1.0f;
    float m_elapsed = 0.0f;
    float m_progress = 0.0f;
    bool m_active = false;
    bool m_finished = true;
};