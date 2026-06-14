#include "engine/ui/Button.h"

#include <utility>

namespace
{
    [[nodiscard]] SDL_FRect toSDLRect(const Rectf &rect)
    {
        return SDL_FRect{rect.x, rect.y, rect.w, rect.h};
    }
}

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

void Button::render(SDL_Renderer *renderer) const
{
    if (renderer == nullptr)
    {
        return;
    }

    const SDL_FRect buttonRect = toSDLRect(m_rect);
    SDL_Color fillColor = m_enabled ? SDL_Color{60, 60, 60, 255} : SDL_Color{96, 96, 96, 255};
    SDL_Color borderColor = m_enabled ? SDL_Color{32, 32, 32, 255} : SDL_Color{128, 128, 128, 255};
    SDL_Color textColor = SDL_Color{240, 240, 240, 255};

    if (m_enabled && m_selected)
    {
        fillColor = SDL_Color{200, 40, 40, 255};
        borderColor = SDL_Color{255, 120, 120, 255};
        textColor = SDL_Color{255, 255, 255, 255};
    }

    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderRect(renderer, &buttonRect);

    SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b, textColor.a);
    const float textX = m_rect.x + 16.0f;
    const float textY = m_rect.y + (m_rect.h - 12.0f) * 0.5f;
    SDL_RenderDebugText(renderer, textX, textY, m_text.c_str());
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