#include "engine/ui/Button.h"

#include <algorithm>
#include <utility>

const Font *Button::s_defaultFont = nullptr;

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
    if (m_enabled && m_onClick)
        m_onClick();

    return m_enabled;
}

void Button::render(Renderer *renderer) const
{
    if (!renderer)
        return;

    Color fillColor = m_enabled ? m_normalColor : Color{96, 96, 96, 255};
    Color borderColor = m_enabled ? Color{32, 32, 32, 255} : Color{128, 128, 128, 255};
    Color textColor = Color{240, 240, 240, 255};

    if (m_enabled && m_selected)
    {
        fillColor = m_selectedColor;
        borderColor = Color{255, 120, 120, 255};
        textColor = Color{255, 255, 255, 255};
    }

    renderer->setDrawColor(fillColor);
    renderer->fillRect(m_rect);

    renderer->setDrawColor(borderColor);
    renderer->drawRect(m_rect);

    renderer->setDrawColor(textColor);
    if (s_defaultFont)
    {
        const Rectf contentRect{
            m_rect.x + m_padding.left,
            m_rect.y + m_padding.top,
            std::max(0.0f, m_rect.w - (m_padding.left + m_padding.right)),
            std::max(0.0f, m_rect.h - (m_padding.top + m_padding.bottom)),
        };
        renderer->renderTextInRect(
            s_defaultFont,
            m_text,
            contentRect,
            textColor,
            m_textAlignH,
            m_textAlignV,
            false,
            false,
            false);
    }
    else
    {
        renderer->drawDebugText(
            {m_rect.x + m_padding.left,
             m_rect.y + (m_rect.h - 12.0f) * 0.5f},
            m_text.c_str());
    }
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
