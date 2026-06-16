#include <algorithm>
#include "engine/ui/Slider.h"

// [x]: Helper: clamp value to [min, max] (kept local to avoid extra dependencies)
static float clamp(float v, float lo, float hi)
{
    return std::clamp(v, lo, hi);
}

// [x]: Sets the screen-space rectangle of the slider track (UI layout input)
void Slider::setTrackRect(Rectf track)
{
    m_track = track;
}

// [x]: Defines logical value range of the slider (e.g. 0–100, 0–1, etc.)
//      Ensures current value stays valid after range changes.
void Slider::setRange(float min, float max)
{
    if (min > max)
        std::swap(min, max);

    m_min = min;
    m_max = max;

    m_value = clamp(m_value, m_min, m_max);
}

// [x]: Sets slider value in logical range [m_min, m_max], clamped safely.
void Slider::setValue(float value)
{
    m_value = clamp(value, m_min, m_max);
}

// [x]: Returns current logical slider value.
float Slider::getValue() const
{
    return m_value;
}

// [x]: Converts mouse X position into slider value while dragging.
//      UI layer responsibility: only active when dragging is true.
void Slider::handleDrag(int mouseX, int mouseY, bool dragging)
{
    (void)mouseY;

    if (!dragging || m_track.w <= 0.0f)
        return;

    float t = (mouseX - m_track.x) / m_track.w;
    t = clamp(t, 0.0f, 1.0f);

    setValue(m_min + t * (m_max - m_min));
}

// [x]: Renders slider track + handle using normalized value mapping.
//      Render depends on normalized state only (no input logic here).
void Slider::render(Renderer *renderer) const
{
    if (!renderer || m_track.w <= 0.0f || m_max == m_min)
        return;

    // --- Track ---
    SDL_FRect trackRect = {
        m_track.x,
        m_track.y,
        m_track.w,
        m_track.h};

    renderer->setDrawColor(Color{80, 80, 80, 255});
    renderer->fillRect({trackRect.x, trackRect.y, trackRect.w, trackRect.h});

    // --- Normalized value [0..1] ---
    const float t = normalized();

    // --- Handle position ---
    const float handleX = m_track.x + t * m_track.w - 5.0f;

    SDL_FRect handleRect = {
        handleX,
        m_track.y - 2.0f,
        10.0f,
        m_track.h + 4.0f};

    renderer->setDrawColor(Color{220, 220, 220, 255});
    renderer->fillRect({handleRect.x, handleRect.y, handleRect.w, handleRect.h});
}

// [x]: Increments/decrements value by delta (used for keyboard/controller input)
void Slider::step(float delta)
{
    setValue(m_value + delta);
}

// [x]: Converts value to normalized [0..1] space for rendering.
//      Safe against zero-length ranges.
float Slider::normalized() const
{
    if (m_max == m_min)
        return 0.0f;

    return (m_value - m_min) / (m_max - m_min);
}