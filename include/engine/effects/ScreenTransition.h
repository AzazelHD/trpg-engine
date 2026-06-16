#pragma once

#include "engine/math/MathUtils.h"
#include "engine/renderer/Color.h"
#include "engine/renderer/Renderer.h"

#include <functional>

class ScreenTransition
{
public:
    struct State
    {
        float alpha = 0.f;
    };

    using EasingFn = std::function<float(float)>;
    using TransitionFn = std::function<State(float)>;

    struct Config
    {
        TransitionFn transition;
        float duration = 0.5f;
        FColor color = {0.f, 0.f, 0.f, 1.f};
        EasingFn easing = easeInOut;
        std::function<void()> onComplete = nullptr;
    };

    void start(Config config);

    void update(float dt);

    void render(Renderer *renderer, float viewW, float viewH) const;

    void reset();

    bool isActive() const { return m_active; }

    float progress() const { return m_progress; }

private:
    Config m_config;

    float m_progress = 0.f;
    bool m_active = false;

    State m_state;
};

namespace ScreenTransitions
{
    inline const ScreenTransition::TransitionFn FadeIn =
        [](float t) -> ScreenTransition::State
    {
        return {1.f - t};
    };

    inline const ScreenTransition::TransitionFn FadeOut =
        [](float t) -> ScreenTransition::State
    {
        return {t};
    };

    // TODO: add more transition types (e.g. wipes, zooms, etc.) as needed.
}