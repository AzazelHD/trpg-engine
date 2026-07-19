// SliderControl.h
#pragma once

#include "engine/ui/IRowControl.h"
#include "engine/ui/Slider.h"

class SliderControl : public IRowControl
{
public:
    explicit SliderControl(Slider *slider, float step = 0.03f)
        : m_slider(slider), m_step(step)
    {
    }

    float measureWidth(Renderer *renderer, const Font *font, float ui) const override;
    void render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                Color normalColor, Color selectedColor) const override;

    bool activate() const override { return false; } // sliders don't "activate"
    void setSelected(bool selected) override { m_selected = selected; }
    bool isEnabled() const override { return true; }

    bool handleLeft() override;
    bool handleRight() override;

private:
    Slider *m_slider;
    float m_step;
    bool m_selected = false;
};