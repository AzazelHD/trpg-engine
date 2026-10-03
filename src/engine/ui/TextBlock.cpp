#include "engine/ui/TextBlock.h"

#include "engine/renderer/Renderer.h"
#include "engine/ui/TextWrap.h"
#include "engine/ui/VerticalLayout.h"

#include <algorithm>
#include <vector>

TextBlock::Layout TextBlock::measure(Renderer &renderer,
                                     const std::vector<Line> &lines,
                                     float maxWidth,
                                     float spacing)
{
    Layout layout;
    const float wrapWidth = std::max(1.0f, maxWidth);

    std::vector<VerticalLayout::Item> items;
    items.reserve(lines.size() * 2);

    float maxRowWidth = 0.0f;

    for (const Line &line : lines)
    {
        if (line.font == nullptr || line.text.empty())
            continue;

        const std::vector<std::string> wrapped =
            TextWrap::wrap(&renderer, line.font, line.text, wrapWidth);

        for (const std::string &row : wrapped)
        {
            const float rowWidth = row.empty() ? 0.0f : renderer.measureText(line.font, row, line.bold).x;
            const float rowHeight = std::max(
                1.0f,
                row.empty() ? renderer.measureText(line.font, "Ag", line.bold).y
                            : renderer.measureText(line.font, row, line.bold).y);

            items.push_back(VerticalLayout::Item{rowWidth, rowHeight, {}});
            maxRowWidth = std::max(maxRowWidth, rowWidth);
        }
    }

    if (items.empty())
        return layout;

    if (!items.empty())
        items.back().margin.bottom = 0.0f;

    for (auto it = items.begin(); it != items.end() - 1; ++it)
        it->margin.bottom = spacing;

    const float totalHeight = VerticalLayout::measureTotalHeight(items);

    layout.box = Rectf{0.0f, 0.0f, maxRowWidth, totalHeight};
    layout.rowRects = VerticalLayout::layoutColumn(items, Vec2f{0.0f, 0.0f});
    return layout;
}

Vec2f TextBlock::anchoredOrigin(const Layout &layout,
                                const Rectf &area,
                                Anchor anchor)
{
    const float blockWidth = layout.width();
    const float blockHeight = layout.height();

    const bool leftCol = anchor == Anchor::TopLeft || anchor == Anchor::Left || anchor == Anchor::BottomLeft;
    const bool rightCol = anchor == Anchor::TopRight || anchor == Anchor::Right || anchor == Anchor::BottomRight;

    const bool topRow = anchor == Anchor::TopLeft || anchor == Anchor::Top || anchor == Anchor::TopRight;
    const bool bottomRow = anchor == Anchor::BottomLeft || anchor == Anchor::Bottom || anchor == Anchor::BottomRight;

    float x = area.x;
    if (leftCol)
        x = area.x;
    else if (rightCol)
        x = area.x + area.w - blockWidth;
    else
        x = area.x + (area.w - blockWidth) * 0.5f;

    float y = area.y;
    if (topRow)
        y = area.y;
    else if (bottomRow)
        y = area.y + area.h - blockHeight;
    else
        y = area.y + (area.h - blockHeight) * 0.5f;

    const float minX = area.x;
    const float maxX = std::max(minX, area.x + area.w - blockWidth);
    const float minY = area.y;
    const float maxY = std::max(minY, area.y + area.h - blockHeight);

    x = std::clamp(x, minX, maxX);
    y = std::clamp(y, minY, maxY);

    return Vec2f{x, y};
}

float TextBlock::render(Renderer &renderer,
                        const std::vector<Line> &lines,
                        const Rectf &area,
                        Anchor anchor,
                        float maxWidth,
                        float spacing,
                        HorizontalAlign align,
                        bool clipToArea)
{
    const Layout layout = measure(renderer, lines, maxWidth, spacing);
    if (layout.rowRects.empty())
        return 0.0f;

    const Vec2f origin = anchoredOrigin(layout, area, anchor);
    const float wrapWidth = std::max(1.0f, maxWidth);
    const float clipBottom = area.y + area.h;

    std::size_t rowIndex = 0;

    for (const Line &line : lines)
    {
        if (line.font == nullptr || line.text.empty())
            continue;

        const std::vector<std::string> wrapped =
            TextWrap::wrap(&renderer, line.font, line.text, wrapWidth);

        for (const std::string &row : wrapped)
        {
            if (rowIndex >= layout.rowRects.size())
                break;

            const Rectf &rowRect = layout.rowRects[rowIndex++];

            if (row.empty())
                continue;

            if (clipToArea && origin.y + rowRect.y + rowRect.h > clipBottom)
                continue;

            // Rows align against the AREA's left edge (area.x), not the block
            // origin, so combining a centered-column anchor (e.g. Bottom) with
            // HorizontalAlign::Center centers rows once — not twice.
            float rowX = area.x;
            if (align == HorizontalAlign::Center)
                rowX = area.x + (wrapWidth - rowRect.w) * 0.5f;
            else if (align == HorizontalAlign::Right)
                rowX = area.x + wrapWidth - rowRect.w;

            renderer.renderText(line.font, row,
                                Vec2f{rowX, origin.y + rowRect.y},
                                line.color, line.bold);
        }
    }

    return layout.height();
}