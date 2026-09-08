#pragma once

#include "engine/animation/AnimationState.h"

#include <algorithm>

// One-shot state that plays exactly one duration, then reports finished — the
// exit-time leaf for timed effects/beats declared in an Animator graph (see
// Animator::addTransition with an empty trigger).
//
// progress() exposes the 0→1 playback position so the game can drive an eased
// visual from the same timer that owns the state's lifetime, instead of
// re-implementing a countdown next to the Animator.
class TimedState final : public AnimationState
{
public:
    explicit TimedState(float duration) : m_duration(std::max(0.0f, duration)) {}

    void onEnter() override { m_elapsed = 0.0f; }

    void update(float dt) override
    {
        if (!isFinished())
            m_elapsed += std::max(dt, 0.0f);
    }

    [[nodiscard]] bool isFinished() const override
    {
        return m_duration <= 0.0f || m_elapsed >= m_duration;
    }

    // [0, 1] over the state's lifetime; 1 once finished or for zero-duration states.
    [[nodiscard]] float progress() const
    {
        return m_duration <= 0.0f ? 1.0f : std::min(m_elapsed / m_duration, 1.0f);
    }

private:
    float m_duration;
    float m_elapsed = 0.0f;
};