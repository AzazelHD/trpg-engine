#pragma once

#include "engine/ui/IRowControl.h"

#include <functional>
#include <string>

class ButtonControl : public IRowControl
{
public:
    using LabelFormatter = std::function<std::string(const std::string &, bool)>;

    ButtonControl(std::function<std::string()> getLabel, std::function<void()> onClick)
        : m_getLabel(std::move(getLabel)), m_onClick(std::move(onClick))
    {
    }

    void setLabelFormatter(LabelFormatter fmt) { m_labelFormatter = std::move(fmt); }

    float measureWidth(Renderer *, const Font *, float) const override { return 0.0f; }
    void render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                Color normalColor, Color selectedColor) const override;

    bool activate() const override
    {
        if (m_onClick)
            m_onClick();
        return true;
    }
    void setSelected(bool selected) override { m_selected = selected; }
    bool isEnabled() const override { return true; }
    bool isFullWidth() const override { return true; }

private:
    std::function<std::string()> m_getLabel;
    std::function<void()> m_onClick;
    LabelFormatter m_labelFormatter;
    bool m_selected = false;
};