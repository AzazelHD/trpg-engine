#include "engine/input/Input.h"
#include "engine/input/KeyCode.h" // needed for KeyCode in update()
#include "engine/ui/MenuPanel.h"
#include "engine/renderer/Renderer.h"

// [x] Adds button (copy version), applies panel offset and selection refresh
void MenuPanel::addButton(const Button &button)
{
    Button copy = button;
    if (!m_hasLayout)
    {
        copy.translate(m_position);
    }
    m_buttons.push_back(copy);
    refreshLayout();
    rebuildFocus();
}

// [x] Adds button (move version), applies panel offset and selection refresh
void MenuPanel::addButton(Button &&button)
{
    if (!m_hasLayout)
    {
        button.translate(m_position);
    }

    m_buttons.push_back(std::move(button));
    refreshLayout();
    rebuildFocus();
}

// [x] Handles keyboard input for menu navigation only
void MenuPanel::handleInput()
{
    // Single point of keyboard navigation – no other scene should read
    // Up/Down/WASD directly.  If you need to add game-specific input, do it
    // in the scene's handleInput() but only for actions *other than* menu
    // movement.
    const Input &input = Input::instance();

    // Use allowRepeat = true so users can hold arrows to scroll smoothly!
    bool moveUp = input.isKeyPressed(KeyCode::Up, true) || input.isKeyPressed(KeyCode::W, true);
    bool moveDown = input.isKeyPressed(KeyCode::Down, true) || input.isKeyPressed(KeyCode::S, true);

    if (moveUp != moveDown)
    {
        if (moveUp)
            navigateUp();
        else
            navigateDown();
    }
}

// [x] Full update: navigation + Accept/Back detection (recommended)
void MenuPanel::update()
{
    // 1. Navigation (Up/Down/WASD)
    handleInput();

    const Input &input = Input::instance();

    // 2. Activation (Accept)
    if (input.isKeyPressed(KeyCode::Accept, false))
    {
        (void)activateSelected(); // fire callback, discard return
    }
    // 3. Cancel (Back)
    else if (input.isKeyPressed(KeyCode::Back, false))
    {
        if (m_onCancel)
            m_onCancel();
    }
}

// [x] Renders background (if set) and all buttons
void MenuPanel::render(Renderer *renderer) const
{
    // Optional background panel
    if (m_bgRect.w > 0.f && m_bgRect.h > 0.f)
    {
        renderer->setBlendMode(Renderer::BlendMode::Blend);
        renderer->setDrawColor(m_bgColor);
        renderer->fillRect(m_bgRect);
        // Border
        renderer->setDrawColor(Color{180, 180, 180, 255});
        renderer->drawRect(m_bgRect);
    }

    // Buttons
    for (const Button &button : m_buttons)
    {
        button.render(renderer);
    }
}

// [x] Sets absolute panel position (applies offset to all buttons)
void MenuPanel::setPosition(Vec2f position)
{
    if (m_hasLayout)
    {
        m_position = position;
        refreshLayout();
        m_focus.refresh();
        return;
    }

    const Vec2f delta = position - m_position;
    m_position = position;

    for (Button &button : m_buttons)
    {
        button.translate(delta);
    }

    m_focus.refresh();
}

void MenuPanel::setVerticalLayout(const VerticalLayoutConfig &layout)
{
    m_layout = layout;
    m_hasLayout = true;
    refreshLayout();
    m_focus.refresh();
}

void MenuPanel::clearLayout()
{
    m_hasLayout = false;
}

// [x] Moves selection up with wrap-around
void MenuPanel::navigateUp()
{
    m_focus.focusPrevious();
}

// [x] Moves selection down with wrap-around
void MenuPanel::navigateDown()
{
    m_focus.focusNext();
}

// [x] Activates currently selected button if valid
bool MenuPanel::activateSelected()
{
    return m_focus.activateSelected();
}

// [x] Returns current selected index
int MenuPanel::getSelectedIndex() const
{
    return m_focus.getSelectedIndex();
}

// [x] Returns true if no buttons exist
bool MenuPanel::empty() const
{
    return m_focus.empty();
}

// [x] Returns number of buttons in panel
std::size_t MenuPanel::size() const
{
    return m_focus.size();
}

void MenuPanel::rebuildFocus()
{
    m_focus.reset(m_buttons);
}

void MenuPanel::refreshLayout()
{
    if (m_hasLayout)
    {
        VerticalLayout::apply(m_buttons, m_position, m_layout);
    }
}