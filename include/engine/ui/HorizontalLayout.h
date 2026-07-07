#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Insets.h"

#include <vector>

// Row-based sibling of VerticalLayout: stacks items left-to-right instead of
// top-to-bottom. Same pure-geometry approach — a list of {width, height,
// margin} boxes in, a list of Rectf out.
//
// Each item's margin.left/right controls horizontal gaps before/after it
// (like CSS margin-left/margin-right in a normal flow row); margin.top
// nudges that single item down from the row's origin.y.
class HorizontalLayout
{
public:
    struct Item
    {
        float width = 0.0f;
        float height = 0.0f;
        Insets margin{}; // only .top/.left/.right are used here
    };

    static std::vector<Rectf> stack(const std::vector<Item> &items, Vec2f origin)
    {
        std::vector<Rectf> out;
        out.reserve(items.size());

        float currentX = origin.x;
        for (const Item &item : items)
        {
            currentX += item.margin.left;
            out.push_back(Rectf{currentX, origin.y + item.margin.top, item.width, item.height});
            currentX += item.width + item.margin.right;
        }

        return out;
    }
};