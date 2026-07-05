#pragma once

#include "engine/math/Rect.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Insets.h"

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
};