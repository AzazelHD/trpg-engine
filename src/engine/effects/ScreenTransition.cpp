#include "engine/effects/ScreenTransition.h"

#include <algorithm>
#include <utility>

void ScreenTransition::start(Config config)
{
    m_config = std::move(config);

    if (!m_config.transition)
    {
        m_config.transition = ScreenTransitions::FadeOut;
    }

    m_progress = 0.f;

    const float eased =
        m_config.easing
            ? std::clamp(m_config.easing(0.f), 0.f, 1.f)
            : 0.f;

    m_state = m_config.transition(eased);

    m_active = true;
}

void ScreenTransition::update(float dt)
{
    if (!m_active)
    {
        return;
    }

    const float duration = std::max(m_config.duration, 0.0001f);

    m_progress += dt / duration;
    m_progress = std::min(m_progress, 1.f);

    const float eased =
        m_config.easing
            ? std::clamp(m_config.easing(m_progress), 0.f, 1.f)
            : m_progress;

    m_state = m_config.transition(eased);

    if (m_progress >= 1.f)
    {
        m_active = false;

        if (m_config.onComplete)
        {
            m_config.onComplete();
        }
    }
}

void ScreenTransition::render(
    SDL_Renderer *renderer,
    float viewW,
    float viewH) const
{
    if (m_state.alpha <= 0.f)
    {
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColorFloat(
        renderer,
        m_config.color.r,
        m_config.color.g,
        m_config.color.b,
        m_config.color.a * m_state.alpha);

    SDL_FRect rect{
        0.f,
        0.f,
        viewW,
        viewH};

    SDL_RenderFillRect(renderer, &rect);
}

void ScreenTransition::reset()
{
    m_progress = 0.f;
    m_state = {};
    m_active = false;
    m_config.onComplete = nullptr;
}