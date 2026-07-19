#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Insets.h"

#include <algorithm>
#include <vector>

enum class HorizontalAlignment
{
    Left,
    Center,
    Right
};

struct VerticalLayoutConfig
{
    Insets padding{};
    float spacing = 0.0f;
    float width = 0.0f;
    HorizontalAlignment alignment = HorizontalAlignment::Left;
};

// Column-based sibling of HorizontalLayout.
//
// Provides two APIs:
//
//  • apply()         -> positions real widget objects
//  • geometry API    -> works purely with rectangles/items
class VerticalLayout
{
public:
    template <typename Container>
    static void apply(Container &widgets,
                      Vec2f origin,
                      const VerticalLayoutConfig &config = {})
    {
        const float contentX = origin.x + config.padding.left;

        const float contentWidth =
            config.width > 0.0f
                ? std::max(0.0f,
                           config.width - config.padding.left - config.padding.right)
                : 0.0f;

        float currentY = origin.y + config.padding.top;

        for (auto &widget : widgets)
        {
            Rectf rect = widget.getRect();

            float x = contentX;

            if (contentWidth > 0.0f)
            {
                switch (config.alignment)
                {
                case HorizontalAlignment::Center:
                    x += (contentWidth - rect.w) * 0.5f;
                    break;

                case HorizontalAlignment::Right:
                    x += contentWidth - rect.w;
                    break;

                default:
                    break;
                }
            }

            widget.setPosition({x, currentY});

            currentY += rect.h + config.spacing;
        }
    }

    struct Item
    {
        float width = 0.0f;
        float height = 0.0f;
        Insets margin{};
    };

    struct Container
    {
        std::vector<Item> items;
        Insets padding{};
    };

    struct ContainerResult
    {
        Rectf box;
        std::vector<Rectf> itemRects;
    };

    static float measureTotalHeight(const std::vector<Item> &items)
    {
        float total = 0.0f;

        for (const Item &item : items)
            total += item.margin.top + item.height + item.margin.bottom;

        return total;
    }

    // Bounding box enclosing every child item plus padding.
    static Rectf computeBounds(const std::vector<Item> &items,
                               Vec2f origin,
                               Insets padding)
    {
        float maxWidth = 0.0f;

        for (const Item &item : items)
            maxWidth = std::max(maxWidth,
                                item.margin.left + item.width);

        return Rectf{
            origin.x - padding.left,
            origin.y - padding.top,
            maxWidth + padding.left + padding.right,
            measureTotalHeight(items) + padding.top + padding.bottom};
    }

    // Position every item inside one vertical container.
    static std::vector<Rectf> layoutColumn(const std::vector<Item> &items,
                                           Vec2f origin)
    {
        std::vector<Rectf> out;
        out.reserve(items.size());

        float currentY = origin.y;

        for (const Item &item : items)
        {
            currentY += item.margin.top;

            out.push_back({origin.x + item.margin.left,
                           currentY,
                           item.width,
                           item.height});

            currentY += item.height + item.margin.bottom;
        }

        return out;
    }

    // Position several independent containers in one column.
    static std::vector<ContainerResult> layoutContainers(const std::vector<Container> &containers,
                                                         Vec2f origin,
                                                         float gap)
    {
        std::vector<ContainerResult> out;
        out.reserve(containers.size());

        float currentY = origin.y;

        for (const Container &container : containers)
        {
            const Vec2f contentOrigin{
                origin.x + container.padding.left,
                currentY + container.padding.top};

            const Rectf box =
                computeBounds(container.items,
                              contentOrigin,
                              container.padding);

            out.push_back({box,
                           layoutColumn(container.items, contentOrigin)});

            currentY = box.y + box.h + gap;
        }

        return out;
    }
};