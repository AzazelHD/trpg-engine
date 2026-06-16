#include "engine/ui/Button.h"

#include <utility>

Button::Button(Rectf rect, std::string text)
    : m_rect(rect),
      m_text(std::move(text)),
      m_normalColor{220, 220, 220, 255},
      m_selectedColor{255, 255, 255, 255},
      m_enabled(true),
      m_selected(false)
{
}

bool Button::activate() const
{
    return m_enabled;
}

void Button::render(Renderer *renderer) const
{
    if (!renderer)

        return;

    Color fillColor = m_enabled ? Color{60, 60, 60, 255} : Color{96, 96, 96, 255};
    Color borderColor = m_enabled ? Color{32, 32, 32, 255} : Color{128, 128, 128, 255};
    Color textColor = Color{240, 240, 240, 255};

    if (m_enabled && m_selected)
    {
        fillColor = Color{200, 40, 40, 255};
        borderColor = Color{255, 120, 120, 255};
        textColor = Color{255, 255, 255, 255};
    }

    renderer->setDrawColor(fillColor);
    renderer->fillRect(m_rect);

    renderer->setDrawColor(borderColor);
    renderer->drawRect(m_rect);

    renderer->setDrawColor(textColor);
    const float textX = m_rect.x + 16.0f;
    const float textY = m_rect.y + (m_rect.h - 12.0f) * 0.5f;
    renderer->drawDebugText({textX, textY}, m_text.c_str());
}

void Button::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if (!m_enabled)
    {
        m_selected = false;
    }
}

bool Button::isEnabled() const
{
    return m_enabled;
}

void Button::setSelected(bool selected)
{
    m_selected = m_enabled && selected;
}

bool Button::isSelected() const
{
    return m_selected;
}

void Button::setPosition(Vec2f position)
{
    m_rect.x = position.x;
    m_rect.y = position.y;
}

void Button::translate(Vec2f delta)
{
    m_rect.x += delta.x;
    m_rect.y += delta.y;
}

Rectf Button::getRect() const
{
    return m_rect;
}