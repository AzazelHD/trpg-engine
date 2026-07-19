// ButtonControl.h
#pragma once

#include "engine/ui/IRowControl.h"

#include <functional>
#include <string>

class ButtonControl : public IRowControl
{
public:
    // ----- existing label getter & action callback -----
    ButtonControl(std::function<std::string()> getLabel, std::function<void()> onAccept)
        : m_getLabel(std::move(getLabel)), m_onAccept(std::move(onAccept))
    {
    }

    // ----- NEW: label formatter type -----
    // Takes the raw label string and a 'selected' flag, returns the final rendered string.
    using LabelFormatter = std::function<std::string(const std::string &, bool)>;

    // ----- NEW: set a game‑specific label formatter -----
    void setLabelFormatter(LabelFormatter fmt)
    {
        m_labelFormatter = std::move(fmt);
    }

    // ----- existing interface (unchanged) -----
    float measureWidth(Renderer *, const Font *, float) const override { return 0.0f; }
    void render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                Color normalColor, Color selectedColor) const override;

    bool activate() const override
    {
        if (m_onAccept)
            m_onAccept();
        return true;
    }
    void setSelected(bool selected) override { m_selected = selected; }
    bool isEnabled() const override { return true; }
    bool isFullWidth() const override { return true; }

private:
    std::function<std::string()> m_getLabel;
    std::function<void()> m_onAccept;
    LabelFormatter m_labelFormatter;
    bool m_selected = false;
};