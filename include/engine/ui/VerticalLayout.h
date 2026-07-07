#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Insets.h"

#include <vector>
#include <algorithm>

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
                ? std::max(0.0f, config.width - config.padding.left - config.padding.right)
                : 0.0f;

        float currentY = origin.y + config.padding.top;

        for (auto &widget : widgets)
        {
            const Rectf rect = widget.getRect();

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

                case HorizontalAlignment::Left:
                default:
                    break;
                }
            }

            widget.setPosition({x, currentY});
            currentY += rect.h + config.spacing;
        }
    }

    // Pure-geometry stacking: no widget objects required, just a list of
    // {width, height, margin} boxes. Each item's own margin (.top/.bottom
    // drive vertical spacing here) is honoured individually. Handy for
    // states that draw rows directly in render() rather than owning
    // persistent widget objects.
    struct Item
    {
        float width = 0.0f;
        float height = 0.0f;
        Insets margin{}; // only .top/.bottom/.left are used here
    };

    static std::vector<Rectf> stack(const std::vector<Item> &items, Vec2f origin)
    {
        std::vector<Rectf> out;
        out.reserve(items.size());

        float currentY = origin.y;
        for (const Item &item : items)
        {
            currentY += item.margin.top;
            out.push_back(Rectf{origin.x + item.margin.left, currentY, item.width, item.height});
            currentY += item.height + item.margin.bottom;
        }

        return out;
    }
};