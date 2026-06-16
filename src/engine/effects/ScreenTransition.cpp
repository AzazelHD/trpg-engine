#include "engine/effects/ScreenTransition.h"
#include "engine/renderer/Renderer.h"

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
    Renderer *renderer,
    float viewW,
    float viewH) const
{
    if (m_state.alpha <= 0.f)
    {
        return;
    }

    const Renderer::BlendMode previousBlendMode = renderer->getBlendMode();
    renderer->setBlendMode(Renderer::BlendMode::Blend);

    renderer->setDrawColor(Color(
        static_cast<uint8_t>(m_config.color.r * 255),
        static_cast<uint8_t>(m_config.color.g * 255),
        static_cast<uint8_t>(m_config.color.b * 255),
        static_cast<uint8_t>(m_config.color.a * m_state.alpha * 255)));

    Rectf rect{
        0.f,
        0.f,
        viewW,
        viewH};

    renderer->fillRect(rect);

    renderer->setBlendMode(previousBlendMode);
}

void ScreenTransition::reset()
{
    m_progress = 0.f;
    m_state = {};
    m_active = false;
    m_config.onComplete = nullptr;
}