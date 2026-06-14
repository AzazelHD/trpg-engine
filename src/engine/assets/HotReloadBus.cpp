#include "engine/assets/HotReloadBus.h"

#include <algorithm>
#include <limits>
#include <utility>

// Assign the next available subscription id to the handler, store it in
// both the id-keyed map and the ordered list, then return the id to the caller.
HotReloadBus::SubscriptionId HotReloadBus::subscribe(Handler handler)
{
    if (!handler)
        return 0;

    std::lock_guard lock(m_mutex);
    const SubscriptionId id = findNextFreeId();
    if (id == 0)
        return 0;

    m_handlers.emplace(id, std::move(handler));
    m_order.push_back(id);
    m_nextId = (id == std::numeric_limits<SubscriptionId>::max()) ? 1 : id + 1;
    return id;
}

// Look up the id in the handler map; if found remove it from both the map
// and the ordered list, then return true. Return false if the id was not found.
bool HotReloadBus::unsubscribe(SubscriptionId id)
{
    std::lock_guard lock(m_mutex);

    const auto handlerIt = m_handlers.find(id);
    if (handlerIt == m_handlers.end())
    {
        return false;
    }

    m_handlers.erase(handlerIt);

    const auto orderIt = std::find(m_order.begin(), m_order.end(), id);
    if (orderIt != m_order.end())
    {
        m_order.erase(orderIt);
    }

    return true;
}

// Take a snapshot of current handlers under the lock, then call each one in
// subscription order without holding the lock. Stop early and return
// DispatchResult::Stop if any handler returns Stop; otherwise return Continue.
DispatchResult HotReloadBus::publish(const HotReloadEvent &event)
{
    const auto handlers = snapshotHandlers();
    for (const auto &handler : handlers)
    {
        if (!handler)
        {
            continue;
        }

        if (handler(event) == DispatchResult::Stop)
        {
            return DispatchResult::Stop;
        }
    }

    return DispatchResult::Continue;
}

// Under the lock, clear the handler map and the ordered id list.
// Do NOT reset m_nextId so that any stale SubscriptionId held by callers
// will not accidentally match a future subscription.
void HotReloadBus::clear()
{
    std::lock_guard lock(m_mutex);
    m_handlers.clear();
    m_order.clear();
}

// Under the lock, build and return a copy of the handlers in subscription order
// so that publish() can iterate them without holding the lock.
std::vector<HotReloadBus::Handler> HotReloadBus::snapshotHandlers() const
{
    std::lock_guard lock(m_mutex);

    std::vector<Handler> handlers;
    handlers.reserve(m_order.size());

    for (const SubscriptionId id : m_order)
    {
        const auto it = m_handlers.find(id);
        if (it != m_handlers.end())
        {
            handlers.push_back(it->second);
        }
    }

    return handlers;
}

HotReloadBus::SubscriptionId HotReloadBus::findNextFreeId() const
{
    SubscriptionId id = m_nextId == 0 ? 1 : m_nextId;
    const SubscriptionId start = id;
    while (m_handlers.count(id))
    {
        id = (id == std::numeric_limits<SubscriptionId>::max()) ? 1 : id + 1;
        if (id == start)
            return 0; // all IDs exhausted
    }
    return id;
}
