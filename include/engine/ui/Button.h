#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/IFocusable.h"
#include "engine/renderer/Color.h"
#include "engine/renderer/Renderer.h"

#include <string>
#include <functional>

class Font;

class Button : public IFocusable
{
public:
    struct Padding
    {
        float left = 16.0f;
        float top = 8.0f;
        float right = 16.0f;
        float bottom = 8.0f;
    };

    Button(Rectf rect, std::string text);

    bool activate() const override;
    void render(Renderer *renderer) const;
    void setEnabled(bool enabled);
    bool isEnabled() const override;
    void setSelected(bool selected) override;
    bool isSelected() const;
    void setPosition(Vec2f position);
    void translate(Vec2f delta);
    [[nodiscard]] Rectf getRect() const;

    void setTextAlignment(Renderer::HorizontalAlign hAlign, Renderer::VerticalAlign vAlign)
    {
        m_textAlignH = hAlign;
        m_textAlignV = vAlign;
    }
    void setPadding(float horizontal, float vertical)
    {
        m_padding.left = horizontal;
        m_padding.right = horizontal;
        m_padding.top = vertical;
        m_padding.bottom = vertical;
    }
    void setPadding(Padding padding) { m_padding = padding; }
    [[nodiscard]] Padding getPadding() const { return m_padding; }

    // callback when the button is activated
    void setOnClick(std::function<void()> callback) { m_onClick = std::move(callback); }

    // static default font used by all buttons
    static void setDefaultFont(const Font *font) { s_defaultFont = font; }

private:
    Rectf m_rect;
    std::string m_text;
    Color m_normalColor;
    Color m_selectedColor;
    bool m_enabled;
    bool m_selected;
    Padding m_padding{};
    Renderer::HorizontalAlign m_textAlignH = Renderer::HorizontalAlign::Center;
    Renderer::VerticalAlign m_textAlignV = Renderer::VerticalAlign::Middle;

    std::function<void()> m_onClick;
    static const Font *s_defaultFont;
};