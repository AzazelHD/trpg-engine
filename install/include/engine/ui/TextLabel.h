#pragma once

#include "engine/math/Vec2.h"
#include "engine/renderer/Color.h"
#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"

#include <string>

// [x] TextLabel UI component for rendering single-line UI text using SDL_ttf.
//     Handles font rendering, color, positioning, and basic style flags (bold/italic/underline).
//     [x] Designed for lightweight UI usage (menus, dialogs, labels).
//     [ ] Supports future extensions for text animation (wiggle, shake, typewriter effects).
//     [ ] Current design is immediate-mode rendering (recreates texture per render call; can be optimized later).
//     [ ] Animation system is separated from rendering state to allow procedural visual effects without touching text content.

struct TextStyle
{
    bool bold = false;
    bool italic = false;
    bool underline = false;
};

struct TextAnimation
{
    float time = 0.f;
    float wiggleStrength = 0.f;
    float shakeStrength = 0.f;
};

class TextLabel
{
public:
    TextLabel() = default;

    void setText(std::string text);
    void setPosition(Vec2f position);
    void setColor(Color color);
    void setFont(Font *font);

    void setStyle(const TextStyle &style);
    void setAnimation(const TextAnimation &animation);

    void update(float dt);
    void render(Renderer *renderer);

private:
    std::string m_text;
    Vec2f m_position{0.f, 0.f};
    Color m_color{255, 255, 255, 255};

    Font *m_font = nullptr;

    TextStyle m_style{};
    TextAnimation m_animation{};
};