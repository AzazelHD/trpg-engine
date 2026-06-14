#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"

enum class HorizontalAlignment
{
    Left,
    Center,
    Right
};

struct VerticalLayoutConfig
{
    float spacing = 0.0f;
    float width = 0.0f;
    HorizontalAlignment alignment = HorizontalAlignment::Left;
};

class VerticalLayout
{
public:
    template <typename Container>
    static void apply(Container &widgets, Vec2f origin, const VerticalLayoutConfig &config = {})
    {
        float currentY = origin.y;

        for (auto &widget : widgets)
        {
            const Rectf rect = widget.getRect();
            float x = origin.x;

            if (config.width > 0.0f)
            {
                switch (config.alignment)
                {
                case HorizontalAlignment::Center:
                    x += (config.width - rect.w) * 0.5f;
                    break;
                case HorizontalAlignment::Right:
                    x += config.width - rect.w;
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
};