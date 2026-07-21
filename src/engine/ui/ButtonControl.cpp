#include "engine/ui/ButtonControl.h"

#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"

void ButtonControl::render(Renderer *renderer, const Font *font, Rectf rect, float /*ui*/,
                           Color normalColor, Color selectedColor) const
{
    if (!renderer || !font || !m_getLabel)
        return;

    const std::string rawLabel = m_getLabel();

    // If the game provided a formatter, use it; otherwise keep the label unchanged.
    const std::string formatted = m_labelFormatter ? m_labelFormatter(rawLabel, m_selected) : rawLabel;

    renderer->renderTextInRect(font, formatted, rect,
                               m_selected ? selectedColor : normalColor,
                               Renderer::HorizontalAlign::Center,
                               Renderer::VerticalAlign::Middle,
                               false, false, false);
}