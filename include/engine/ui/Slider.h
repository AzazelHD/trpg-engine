#pragma once
#include <SDL3/SDL.h>
#include "engine/math/Rect.h"
#include "engine/renderer/Renderer.h"
// [x]: Slider UI component for continuous value input (0–1 or custom range)
//      - Handles value storage + clamping (setValue/getValue)
//      - Converts mouse drag into normalized value (handleDrag)
//      - Renders track + handle via SDL
// TODO: later upgrades → snapping, step increments, keyboard support, theme styling, animation easing
class Slider
{
public:
    void setTrackRect(Rectf track);
    void setRange(float min, float max);
    void setValue(float value);
    void handleDrag(int mouseX, int mouseY, bool dragging);
    void render(Renderer *renderer) const;
    void step(float delta);
    float getValue() const;
    float normalized() const;

private:
    Rectf m_track{};
    float m_min = 0.0f;
    float m_max = 1.0f;
    float m_value = 0.0f;
};