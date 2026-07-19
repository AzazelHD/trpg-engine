// SliderControl.cpp
#include "engine/ui/SliderControl.h"

#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"

#include <cstdio>

namespace
{
    constexpr float kTrackW = 280.0f;
    constexpr float kTrackH = 22.0f;
    constexpr float kPctMarginLeft = 28.0f;
}

float SliderControl::measureWidth(Renderer *renderer, const Font *font, float ui) const
{
    const float pctTextW = (renderer && font) ? renderer->measureText(font, "100%").x : 0.0f;
    return kTrackW * ui + kPctMarginLeft * ui + pctTextW;
}

void SliderControl::render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                           Color normalColor, Color selectedColor) const
{
    if (!renderer || !m_slider)
        return;

    const float trackY = rect.y + (rect.h - kTrackH * ui) * 0.5f;
    const Rectf trackRect{rect.x, trackY, kTrackW * ui, kTrackH * ui};
    m_slider->setTrackRect(trackRect);
    m_slider->render(renderer);

    char pct[16];
    std::snprintf(pct, sizeof(pct), "%d%%",
                  static_cast<int>(m_slider->getValue() * 100.0f + 0.5f));

    const float pctX = rect.x + kTrackW * ui + kPctMarginLeft * ui;
    const Rectf pctRect{pctX, rect.y, rect.x + rect.w - pctX, rect.h};

    renderer->renderTextInRect(font, pct, pctRect,
                               m_selected ? selectedColor : normalColor,
                               Renderer::HorizontalAlign::Left,
                               Renderer::VerticalAlign::Middle,
                               false, false, false);
}

bool SliderControl::handleLeft()
{
    if (!m_slider)
        return false;
    m_slider->step(-m_step);
    return true;
}

bool SliderControl::handleRight()
{
    if (!m_slider)
        return false;
    m_slider->step(m_step);
    return true;
}