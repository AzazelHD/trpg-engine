#pragma once

// HotReloadBus is a thread-safe event bus that broadcasts asset-change notifications
// to any engine system that has subscribed to hot-reload events.
// Handlers are invoked on the thread that calls publish(), with no lock held,
// so handlers may safely call subscribe/unsubscribe/publish without deadlocking.

#include <mutex>
#include <vector>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <unordered_map>

enum class HotReloadEventType
{
    Added,
    Modified,
    Removed,
    Renamed,
};

struct HotReloadEvent
{
    HotReloadEventType type = HotReloadEventType::Modified;
    std::filesystem::path assetPath; // new path (for Renamed: the new name)
    std::filesystem::path oldPath;   // only valid for Renamed events
};

enum class DispatchResult
{
    Continue, // keep dispatching to the next handler
    Stop,     // halt dispatch; subsequent handlers are skipped
};

// Thread-safe event bus for hot-reload notifications.
//
// Threading contract:
//   - subscribe / unsubscribe / publish / clear may be called from any thread.
//   - Handlers are invoked on whichever thread calls publish(), with NO lock held.
//     Handlers may therefore safely call subscribe / unsubscribe / publish.
//
// Dispatch order:
//   - Handlers are dispatched in subscription order (FIFO).
//
// Propagation:
//   - A handler returning DispatchResult::Stop halts dispatch immediately.
//   - publish() returns DispatchResult::Stop if any handler stopped propagation,
//     DispatchResult::Continue if all handlers ran.
class HotReloadBus
{
public:
    using Handler = std::function<DispatchResult(const HotReloadEvent &)>;
    using SubscriptionId = std::size_t;

    HotReloadBus() = default;
    ~HotReloadBus() = default;

    // Non-copyable, non-movable (mutex is not movable)
    HotReloadBus(const HotReloadBus &) = delete;
    HotReloadBus &operator=(const HotReloadBus &) = delete;
    HotReloadBus(HotReloadBus &&) = delete;
    HotReloadBus &operator=(HotReloadBus &&) = delete;

    // Register a handler. Returns a stable ID used for unsubscription.
    SubscriptionId subscribe(Handler handler);

    // Remove a handler by ID. Returns true if the ID was found, false if already gone.
    bool unsubscribe(SubscriptionId id);

    // Dispatch event to all handlers in subscription order.
    // Returns DispatchResult::Stop if a handler halted propagation,
    // DispatchResult::Continue if all handlers ran.
    DispatchResult publish(const HotReloadEvent &event);

    // Remove all subscriptions.
    void clear();

private:
    std::vector<Handler> snapshotHandlers() const;
    [[nodiscard]] SubscriptionId findNextFreeId() const;

    SubscriptionId m_nextId = 1;

    // Parallel structures: map for O(1) erase, vector for stable insertion order.
    std::unordered_map<SubscriptionId, Handler> m_handlers;
    std::vector<SubscriptionId> m_order;

    mutable std::mutex m_mutex;
};
