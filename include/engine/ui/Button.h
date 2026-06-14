#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/IFocusable.h"

class Button : public IFocusable
{
public:
    Button(Rectf rect, std::string text);

    bool activate() const override;
    void render(SDL_Renderer *renderer) const;
    void setEnabled(bool enabled);
    bool isEnabled() const override;
    void setSelected(bool selected) override;
    bool isSelected() const;
    void setPosition(Vec2f position);
    void translate(Vec2f delta);
    [[nodiscard]] Rectf getRect() const;

private:
    Rectf m_rect;
    std::string m_text;
    SDL_Color m_normalColor;
    SDL_Color m_selectedColor;
    bool m_enabled;
    bool m_selected;
};
