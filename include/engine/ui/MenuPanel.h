#pragma once

#include "engine/math/Vec2.h"
#include "engine/ui/Button.h"
#include "engine/ui/FocusGroup.h"
#include "engine/ui/Insets.h"
#include "engine/ui/VerticalLayout.h"

#include <functional>
#include <vector>

class Renderer;

// MenuPanel manages a vertical list of selectable buttons.
//
// Handles navigation, selection state, and activation for UI menus.
// Now also supports a background panel and Cancel (Back) callback.
//
// --- BEHAVIOUR ---
// - [x] Only one button is selected at a time.
// - [x] Selection wraps around when moving past edges.
// - [x] Empty state is safe (guarded in implementation).
// - [x] Selection visuals are synced through FocusGroup.
// - [x] A solid background rectangle can be drawn behind the buttons.
//
// --- INPUT FLOW ---
// - [x] update() autonomously queries the global Input instance and
//       handles navigation (Up/Down), activation (Accept → activateSelected()),
//       and cancellation (Back → onCancel callback).
//       This is the single update entry point for the panel.
// - [x] handleInput() still exists but only does navigation (for backwards
//       compatibility or custom use).
//
// --- POSITION ---
// - [x] m_position defines panel origin
// - [x] Optional vertical layout helper supports spacing and alignment

class MenuPanel
{
public:
    MenuPanel() = default;

    void addButton(const Button &button);
    void addButton(Button &&button);

    // Full update: navigation + Accept/Back detection
    void update();

    void render(Renderer *renderer) const;
    void setPosition(Vec2f position);
    void setVerticalLayout(const VerticalLayoutConfig &layout);
    void clearLayout();

    void setPadding(Insets padding)
    {
        m_padding = padding;
        refreshLayout();
    }

    [[nodiscard]] Insets getPadding() const
    {
        return m_padding;
    }

    void navigateUp();
    void navigateDown();

    [[nodiscard]] bool activateSelected();
    [[nodiscard]] int getSelectedIndex() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::size_t size() const;

    void setOnCancel(std::function<void()> callback)
    {
        m_onCancel = std::move(callback);
    }

    void clearButtons()
    {
        m_buttons.clear();
        rebuildFocus();
    }

    void setBackground(Rectf rect, Color color)
    {
        m_bgRect = rect;
        m_bgColor = color;
    }

    // Access buttons for enabling/disabling individual items
    [[nodiscard]] std::vector<Button> &getButtons() { return m_buttons; }
    [[nodiscard]] const std::vector<Button> &getButtons() const { return m_buttons; }

private:
    void rebuildFocus();
    void refreshLayout();

    std::vector<Button> m_buttons;
    FocusGroup m_focus;

    Vec2f m_position{0.f, 0.f};

    VerticalLayoutConfig m_layout{.spacing = 8.0f};
    bool m_hasLayout = true;

    Insets m_padding{};

    std::function<void()> m_onCancel;

    Rectf m_bgRect{};
    Color m_bgColor{0, 0, 0, 0};
};
