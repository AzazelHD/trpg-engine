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

    // Bounding box that fully wraps a row of items placed by stack() from
    // origin, expanded by `padding` — a self-sizing container derived FROM
    // its children, instead of a hand-picked width kept in sync by hand.
    static Rectf computeBounds(const std::vector<Item> &items, Vec2f origin, Insets padding)
    {
        float maxH = 0.0f;
        for (const Item &item : items)
            maxH = std::max(maxH, item.margin.top + item.height);

        return Rectf{
            origin.x - padding.left,
            origin.y - padding.top,
            measureTotalWidth(items) + padding.left + padding.right,
            maxH + padding.top + padding.bottom};
    }

    static float measureTotalWidth(const std::vector<Item> &items)
    {
        float total = 0.0f;
        for (const Item &item : items)
            total += item.margin.left + item.width + item.margin.right;
        return total;
    }

    // A named group of items sized as ONE self-sizing box (computeBounds).
    // Several independent Containers — each with its own items/padding —
    // can then sit on the same line via layoutRow(), separated by `gap`.
    // This is the "2 cajas separadas en la misma línea" case: each
    // Container is its own box, not a shared box split into columns.
    struct Container
    {
        std::vector<Item> items;
        Insets padding{};
    };

    struct ContainerResult
    {
        Rectf box;                    // the container's own background box
        std::vector<Rectf> itemRects; // its inner items, already positioned
    };

    static std::vector<ContainerResult> layoutRow(const std::vector<Container> &containers, Vec2f origin, float gap)
    {
        std::vector<ContainerResult> out;
        out.reserve(containers.size());
        float currentX = origin.x;
        for (const Container &c : containers)
        {
            const Vec2f contentOrigin{currentX + c.padding.left, origin.y + c.padding.top};
            const Rectf box = computeBounds(c.items, contentOrigin, c.padding);
            const std::vector<Rectf> itemRects = stack(c.items, contentOrigin);
            out.push_back(ContainerResult{box, itemRects});
            currentX = box.x + box.w + gap;
        }
        return out;
    }
};