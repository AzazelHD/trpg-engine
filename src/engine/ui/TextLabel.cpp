#include <cmath>
#include <SDL3_ttf/SDL_ttf.h>
#include "engine/ui/TextLabel.h"

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
void TextLabel::setColor(SDL_Color color)
{
    m_color = color;
}

// [x]: assign font used for SDL_ttf rendering (must be valid and preloaded)
void TextLabel::setFont(TTF_Font *font)
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
void TextLabel::render(SDL_Renderer *renderer)
{
    if (!renderer || !m_font || m_text.empty())
        return;

    int style = TTF_STYLE_NORMAL;

    if (m_style.bold)
        style |= TTF_STYLE_BOLD;
    if (m_style.italic)
        style |= TTF_STYLE_ITALIC;
    if (m_style.underline)
        style |= TTF_STYLE_UNDERLINE;

    TTF_SetFontStyle(m_font, style);

    SDL_Surface *surface = TTF_RenderText_Blended(m_font, m_text.c_str(), 0, m_color);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture)
        return;

    float offsetX = 0.f;
    float offsetY = 0.f;

    if (m_animation.wiggleStrength > 0.f)
    {
        offsetX = std::sinf(m_animation.time * 10.f) * m_animation.wiggleStrength;
        offsetY = std::cosf(m_animation.time * 12.f) * m_animation.wiggleStrength;
    }

    SDL_FRect dst{
        m_position.x + offsetX,
        m_position.y + offsetY,
        0.f,
        0.f};

    int textWidth = 0;
    int textHeight = 0;
    {
        float tw = 0.f, th = 0.f;
        SDL_GetTextureSize(texture, &tw, &th);
        textWidth = static_cast<int>(tw);
        textHeight = static_cast<int>(th);
    }
    dst.w = static_cast<float>(textWidth);
    dst.h = static_cast<float>(textHeight);
    SDL_RenderTexture(renderer, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
}