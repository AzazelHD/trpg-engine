#include "engine/input/Input.h"
#include "engine/input/KeyCode.h"
#include "engine/ui/MenuPanel.h"
#include "engine/ui/Insets.h"
#include "engine/renderer/Renderer.h"

// Adds button (copy version)
void MenuPanel::addButton(const Button &button)
{
    m_buttons.push_back(button);
    refreshLayout();
    rebuildFocus();
}

// Adds button (move version)
void MenuPanel::addButton(Button &&button)
{
    m_buttons.push_back(std::move(button));
    refreshLayout();
    rebuildFocus();
}

// Input handling
void MenuPanel::update()
{
    const Input &input = Input::instance();

    bool moveUp =
        input.isKeyPressed(KeyCode::Up, true) ||
        input.isKeyPressed(KeyCode::W, true);

    bool moveDown =
        input.isKeyPressed(KeyCode::Down, true) ||
        input.isKeyPressed(KeyCode::S, true);

    if (moveUp != moveDown)
    {
        if (moveUp)
            navigateUp();
        else
            navigateDown();
    }

    if (input.isKeyPressed(KeyCode::Accept, false))
    {
        (void)activateSelected();
    }
    else if (input.isKeyPressed(KeyCode::Back, false))
    {
        if (m_onCancel)
            m_onCancel();
    }
}

// Render
void MenuPanel::render(Renderer *renderer) const
{
    if (m_bgRect.w > 0.f && m_bgRect.h > 0.f)
    {
        renderer->setBlendMode(Renderer::BlendMode::Blend);
        renderer->setDrawColor(m_bgColor);
        renderer->fillRect(m_bgRect);

        renderer->setDrawColor(Color{180, 180, 180, 255});
        renderer->drawRect(m_bgRect);
    }

    for (const Button &button : m_buttons)
    {
        button.render(renderer);
    }
}

// Position
void MenuPanel::setPosition(Vec2f position)
{
    m_position = position;
    refreshLayout();
    m_focus.refresh();
}

// Layout
void MenuPanel::setVerticalLayout(const VerticalLayoutConfig &layout)
{
    m_layout = layout;
    m_hasLayout = true;
    refreshLayout();
    m_focus.refresh();
}

// Clear layout
void MenuPanel::clearLayout()
{
    m_hasLayout = false;
    refreshLayout();
}

// Navigation
void MenuPanel::navigateUp()
{
    m_focus.focusPrevious();
}

void MenuPanel::navigateDown()
{
    m_focus.focusNext();
}

// Actions
bool MenuPanel::activateSelected()
{
    return m_focus.activateSelected();
}

// State
int MenuPanel::getSelectedIndex() const
{
    return m_focus.getSelectedIndex();
}

bool MenuPanel::empty() const
{
    return m_focus.empty();
}

std::size_t MenuPanel::size() const
{
    return m_focus.size();
}

// Focus rebuild
void MenuPanel::rebuildFocus()
{
    m_focus.reset(m_buttons);
}

// Layout engine
void MenuPanel::refreshLayout()
{
    if (!m_hasLayout)
    {
        for (auto &b : m_buttons)
            b.setPosition(m_position);
        return;
    }

    VerticalLayoutConfig layout = m_layout;
    layout.padding = m_padding;

    Vec2f origin = m_position;

    if (m_bgRect.w > 0.f && m_bgRect.h > 0.f)
    {
        origin = {m_bgRect.x, m_bgRect.y};

        if (layout.width <= 0.f)
            layout.width = m_bgRect.w;
    }

    VerticalLayout::apply(m_buttons, origin, layout);
}
