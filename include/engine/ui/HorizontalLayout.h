#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Insets.h"

#include <algorithm>
#include <vector>

// Row-based layout helper.
//
// Works entirely with geometry (no widgets). A row is made of one or more
// independent Containers laid out left-to-right.
//
// Typical usage:
//
//   [ Label box ]   gap   [ Controls box ]
//
// Each Container sizes itself from its children and optional padding.
class HorizontalLayout
{
public:
    // One item inside a container.
    //
    // margin.left/right add horizontal spacing before/after the item.
    // margin.top vertically offsets only this item inside the row.
    struct Item
    {
        float width = 0.0f;
        float height = 0.0f;
        Insets margin{};
    };

    // Self-sizing group of items.
    struct Container
    {
        std::vector<Item> items;
        Insets padding{};
    };

    struct ContainerResult
    {
        Rectf box;                    // container bounds
        std::vector<Rectf> itemRects; // positioned child items
    };

    static float measureTotalWidth(const std::vector<Item> &items)
    {
        float total = 0.0f;

        for (const Item &item : items)
            total += item.margin.left + item.width + item.margin.right;

        return total;
    }

    // Bounding box enclosing every child item plus padding.
    static Rectf computeBounds(const std::vector<Item> &items,
                               Vec2f origin,
                               Insets padding)
    {
        float maxHeight = 0.0f;

        for (const Item &item : items)
            maxHeight = std::max(maxHeight,
                                 item.margin.top + item.height);

        return Rectf{
            origin.x - padding.left,
            origin.y - padding.top,
            measureTotalWidth(items) + padding.left + padding.right,
            maxHeight + padding.top + padding.bottom};
    }

    // Position every item inside one horizontal container.
    static std::vector<Rectf> layoutItems(const std::vector<Item> &items,
                                          Vec2f origin)
    {
        std::vector<Rectf> out;
        out.reserve(items.size());

        float currentX = origin.x;

        for (const Item &item : items)
        {
            currentX += item.margin.left;

            out.push_back({currentX,
                           origin.y + item.margin.top,
                           item.width,
                           item.height});

            currentX += item.width + item.margin.right;
        }

        return out;
    }

    // Position several independent containers on the same row.
    static std::vector<ContainerResult> layoutContainers(const std::vector<Container> &containers,
                                                         Vec2f origin,
                                                         float gap)
    {
        std::vector<ContainerResult> out;
        out.reserve(containers.size());

        float currentX = origin.x;

        for (const Container &container : containers)
        {
            const Vec2f contentOrigin{
                currentX + container.padding.left,
                origin.y + container.padding.top};

            const Rectf box =
                computeBounds(container.items,
                              contentOrigin,
                              container.padding);

            out.push_back({box, layoutItems(container.items, contentOrigin)});

            currentX = box.x + box.w + gap;
        }

        return out;
    }
};