// ValueControl.h
#pragma once

#include "engine/ui/IRowControl.h"

#include <functional>
#include <string>

class ValueControl : public IRowControl
{
public:
    ValueControl(std::function<std::string()> getValue,
                 std::function<void(bool right)> onAdjust)
        : m_getValue(std::move(getValue)), m_onAdjust(std::move(onAdjust))
    {
    }

    float measureWidth(Renderer *renderer, const Font *font, float ui) const override;
    void render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                Color normalColor, Color selectedColor) const override;

    bool activate() const override { return false; }
    void setSelected(bool selected) override { m_selected = selected; }
    bool isEnabled() const override { return true; }

    bool handleLeft() override;
    bool handleRight() override;

private:
    std::function<std::string()> m_getValue;
    std::function<void(bool right)> m_onAdjust;
    bool m_selected = false;
};