#include "engine/ui/TextLabel.h"
#include "engine/renderer/Renderer.h"

#include <cmath>

// [x]: store UTF-8 text and mark as dirty for re-render
void TextLabel::setText(std::string text)
{
    m_text = std::move(text);
}

// [x]: store screen position for rendering (top-left anchor)
void TextLabel::setPosition(Vec2f position)
{
    m_position = position;
}

// [x]: store base RGBA color used during render
void TextLabel::setColor(Color color)
{
    m_color = color;
}

// [x]: assign font used for SDL_ttf rendering (must be valid and preloaded)
void TextLabel::setFont(Font *font)
{
    m_font = font;
}

// [x]: update static text style flags (bold / italic / underline)
void TextLabel::setStyle(const TextStyle &style)
{
    m_style = style;
}

// [x]: update animation state (wiggle, shake, time-based effects)
void TextLabel::setAnimation(const TextAnimation &animation)
{
    m_animation = animation;
}

// [x]: advance animation time (called once per frame)
void TextLabel::update(float dt)
{
    m_animation.time += dt;
}

// [x]: render text using SDL_ttf (surface → texture → screen)
void TextLabel::render(Renderer *renderer)
{
    if (!renderer || !m_font || m_text.empty())
        return;

    float offsetX = 0.f;
    float offsetY = 0.f;

    if (m_animation.wiggleStrength > 0.f)
    {
        offsetX = std::sinf(m_animation.time * 10.f) * m_animation.wiggleStrength;
        offsetY = std::cosf(m_animation.time * 12.f) * m_animation.wiggleStrength;
    }

    renderer->renderText(
        m_font,
        m_text,
        {m_position.x + offsetX, m_position.y + offsetY},
        m_color,
        m_style.bold,
        m_style.italic,
        m_style.underline);
}