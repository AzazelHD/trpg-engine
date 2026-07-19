// ValueControl.cpp
#include "engine/ui/ValueControl.h"

#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"

float ValueControl::measureWidth(Renderer *renderer, const Font *font, float /*ui*/) const
{
    if (!renderer || !font || !m_getValue)
        return 0.0f;
    return renderer->measureText(font, m_getValue()).x;
}

void ValueControl::render(Renderer *renderer, const Font *font, Rectf rect, float /*ui*/,
                          Color normalColor, Color selectedColor) const
{
    if (!renderer || !font || !m_getValue)
        return;

    renderer->renderTextInRect(font, m_getValue(), rect,
                               m_selected ? selectedColor : normalColor,
                               Renderer::HorizontalAlign::Right,
                               Renderer::VerticalAlign::Middle,
                               false, false, false);
}

bool ValueControl::handleLeft()
{
    if (!m_onAdjust)
        return false;
    m_onAdjust(false);
    return true;
}

bool ValueControl::handleRight()
{
    if (!m_onAdjust)
        return false;
    m_onAdjust(true);
    return true;
}