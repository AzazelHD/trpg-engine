#pragma once

#include "engine/math/Rect.h"
#include "engine/renderer/Color.h"
#include "engine/ui/IFocusable.h"

class Font;
class Renderer;

// A row control extends IFocusable so it plugs directly into FocusGroup for
// navigation/selection, like any other focusable widget (Button included).
// It adds the extra capability of measuring its own width and rendering
// itself into an arbitrary rect — needed because SettingsRowWindow's rows
// are heterogeneous (slider, value, button), unlike MenuPanel's uniform
// Button list, which can share one concrete render() call.
class IRowControl : public IFocusable
{
public:
    virtual float measureWidth(Renderer *renderer, const Font *font, float ui) const = 0;

    // Reads its own selected state (set via IFocusable::setSelected)
    // instead of taking a "focused" bool — same convention Button uses.
    virtual void render(Renderer *renderer, const Font *font, Rectf rect, float ui,
                        Color normalColor, Color selectedColor) const = 0;

    virtual bool isFullWidth() const { return false; }
};