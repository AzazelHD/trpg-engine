#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/renderer/Aligment.h"
#include "engine/renderer/Color.h"

#include <string>
#include <vector>

class Font;
class Renderer;

// TextBlock: measure, anchor, and draw a stacked block of styled text lines.
// Gameplay-agnostic. Wraps with TextWrap and stacks rows with VerticalLayout,
// composing the engine's existing layout toolkit instead of duplicating it.
//
// Anchor semantics — content grows AWAY from the anchored edge:
//   Bottom*  → the block rises from the bottom of the area (bottom edge of
//              the block sits on the area's bottom edge when it fits).
//   Top*     → the block descends from the top.
//   Left/Right/Center → horizontal fix: Left edge / Right edge / centered.
//   Middle row anchors (Left, Center, Right) vertically center.
//
// Typical use: a subtitle band bottom-anchored to the screen so long text
// grows upward instead of falling off the window's bottom edge.
namespace TextBlock
{
    enum class Anchor
    {
        TopLeft,
        Top,
        TopRight,
        Left,
        Center,
        Right,
        BottomLeft,
        Bottom,
        BottomRight,
    };

    struct Line
    {
        const Font *font = nullptr;
        std::string text;
        Color color = Color{235, 240, 250, 255};
        bool bold = false;
    };

    struct Layout
    {
        Rectf box{};
        std::vector<Rectf> rowRects;

        float width() const { return box.w; }
        float height() const { return box.h; }
    };

    // Wraps every line to maxWidth and stacks the rows. Row height is sourced
    // from each row's font via Renderer::measureText; rows carry their measured
    // width so callers can align them horizontally. Blank wrapped rows (from
    // explicit \n\n in the source text) reserve space but draw nothing, so
    // authoring gaps survive. Does not draw.
    Layout measure(Renderer &renderer,
                   const std::vector<Line> &lines,
                   float maxWidth,
                   float spacing = 12.0f);

    // Positions an already-measured block inside `area` for the given anchor,
    // clamping so the block stays within `area` whenever it fits. A block
    // larger than the area keeps its anchor edge and clips on the far side.
    Vec2f anchoredOrigin(const Layout &layout,
                         const Rectf &area,
                         Anchor anchor);

    // measure + position + draw in one call. Rows are aligned horizontally
    // inside maxWidth against the AREA's left edge (a centered-column anchor
    // never double-centers), while the anchor only governs vertical placement.
    // Blank wrapped rows reserve space but draw nothing. When clipToArea is
    // true, rows that do not fully fit above the area's bottom edge are
    // skipped (overflow protection for fixed-size cards). Returns the block's
    // total height.
    float render(Renderer &renderer,
                 const std::vector<Line> &lines,
                 const Rectf &area,
                 Anchor anchor,
                 float maxWidth,
                 float spacing = 12.0f,
                 HorizontalAlign align = HorizontalAlign::Left,
                 bool clipToArea = false);
}