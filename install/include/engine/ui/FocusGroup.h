#pragma once

#include <cstddef>
#include <vector>

#include "engine/ui/IFocusable.h"

class FocusGroup
{
public:
    template <typename Container>

    void reset(Container &items)
    {
        int previous = m_selectedIndex;

        m_items.clear();
        m_selectedIndex = -1;

        for (auto &item : items)
        {
            m_items.push_back(&item);
        }

        if (previous >= 0 && previous < static_cast<int>(m_items.size()))
            m_selectedIndex = previous;

        refresh();
    }

    void clear()
    {
        m_items.clear();
        m_selectedIndex = -1;
    }

    void refresh()
    {
        if (m_items.empty())
        {
            m_selectedIndex = -1;
            return;
        }

        if (m_selectedIndex < 0 ||
            m_selectedIndex >= static_cast<int>(m_items.size()) ||
            m_items[m_selectedIndex] == nullptr ||
            !m_items[m_selectedIndex]->isEnabled())
        {
            m_selectedIndex = findFirstEnabledIndex();
        }

        applySelection();
    }

    void focusPrevious()
    {
        if (m_items.empty())
            return;

        m_selectedIndex = findNextEnabledIndex(-1);
        applySelection();
    }

    void focusNext()
    {
        if (m_items.empty())
            return;

        m_selectedIndex = findNextEnabledIndex(1);
        applySelection();
    }

    // When wrap is disabled, focusPrevious/focusNext stop at the nearest
    // enabled item at the ends instead of cycling through the whole group.
    // Enabled by default; menus that should not wrap (e.g. a fixed action
    // list) can turn it off per group.
    void setWrap(bool enabled) { m_wrap = enabled; }
    [[nodiscard]] bool wrap() const { return m_wrap; }

    [[nodiscard]] bool activateSelected() const
    {
        if (m_items.empty())
            return false;

        if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size()))
            return false;

        const IFocusable *item = m_items[m_selectedIndex];
        return item && item->activate();
    }

    // For polymorphic/pointer-owned items (e.g. std::vector<std::unique_ptr<IFocusable>>)
    // that reset()'s address-of-value template can't handle directly.
    void resetFromPointers(std::vector<IFocusable *> items)
    {
        int previous = m_selectedIndex;

        m_items = std::move(items);
        m_selectedIndex = -1;

        if (previous >= 0 && previous < static_cast<int>(m_items.size()))
            m_selectedIndex = previous;

        refresh();
    }

    [[nodiscard]] bool handleSelectedLeft() const
    {
        if (m_items.empty() || m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size()))
            return false;
        IFocusable *item = m_items[m_selectedIndex];
        return item && item->handleLeft();
    }

    [[nodiscard]] bool handleSelectedRight() const
    {
        if (m_items.empty() || m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size()))
            return false;
        IFocusable *item = m_items[m_selectedIndex];
        return item && item->handleRight();
    }

    [[nodiscard]] int getSelectedIndex() const
    {
        return m_selectedIndex;
    }

    [[nodiscard]] bool empty() const
    {
        return m_items.empty();
    }

    [[nodiscard]] std::size_t size() const
    {
        return m_items.size();
    }

private:
    [[nodiscard]] int findFirstEnabledIndex() const
    {
        for (int index = 0; index < static_cast<int>(m_items.size()); ++index)
        {
            const IFocusable *item = m_items[index];
            if (item != nullptr && item->isEnabled())
            {
                return index;
            }
        }

        return -1;
    }

    [[nodiscard]] int findNextEnabledIndex(int direction) const
    {
        if (m_items.empty())
        {
            return -1;
        }

        const int count = static_cast<int>(m_items.size());
        const int currentIndex = m_selectedIndex;

        if (currentIndex < 0 || currentIndex >= count)
        {
            return findFirstEnabledIndex();
        }

        if (!m_wrap)
        {
            const int step = direction < 0 ? -1 : 1;
            for (int candidate = currentIndex + step; candidate >= 0 && candidate < count; candidate += step)
            {
                const IFocusable *item = m_items[candidate];
                if (item != nullptr && item->isEnabled())
                {
                    return candidate;
                }
            }
            return currentIndex; // nothing enabled beyond — stay put, don't wrap
        }

        const int stepDirection = direction < 0 ? -1 : 1;
        int cursor = currentIndex + stepDirection;
        for (int step = 0; step < count; ++step)
        {
            if (cursor < 0)
                cursor = count - 1;
            else if (cursor >= count)
                cursor = 0;

            const IFocusable *item = m_items[cursor];
            if (item != nullptr && item->isEnabled())
            {
                return cursor;
            }
            cursor += stepDirection;
        }

        return -1;
    }

    void applySelection()
    {
        for (int i = 0; i < static_cast<int>(m_items.size()); ++i)
        {
            IFocusable *item = m_items[i];
            if (!item)
                continue;

            item->setSelected(static_cast<int>(i) == m_selectedIndex);
        }
    }

    std::vector<IFocusable *> m_items;
    int m_selectedIndex = -1;
    bool m_wrap = true;
};