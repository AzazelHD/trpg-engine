#pragma once

#include <vector>
#include "engine/math/Vec2.h"
#include "engine/ui/Button.h"
#include "engine/ui/FocusGroup.h"
#include "engine/ui/VerticalLayout.h"
#include "engine/renderer/Renderer.h"

// MenuPanel manages a vertical list of selectable buttons.
//
// Handles navigation, selection state, and activation for UI menus.
//
// --- BEHAVIOUR ---
// - [x] Only one button is selected at a time.
// - [x] Selection wraps around when moving past edges.
// - [x] Empty state is safe (guarded in implementation).
// - [x] Selection visuals are synced through FocusGroup.
//
// --- INPUT FLOW ---
// - [x] handleInput() autonomously queries the global Input instance
//       and routes valid presses to navigateUp / navigateDown.
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

    // Scans engine inputs and updates internal selection
    void handleInput();

    void render(Renderer *renderer) const;
    void setPosition(Vec2f position);
    void setVerticalLayout(const VerticalLayoutConfig &layout);
    void clearLayout();

    void navigateUp();
    void navigateDown();

    [[nodiscard]] bool activateSelected();
    [[nodiscard]] int getSelectedIndex() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::size_t size() const;

private:
    void rebuildFocus();
    void refreshLayout();

    std::vector<Button> m_buttons;
    FocusGroup m_focus;
    Vec2f m_position{0.f, 0.f};
    VerticalLayoutConfig m_layout{};
    bool m_hasLayout = false;
};