#include "engine/assets/FileWatcher.h"
#include <algorithm>

// ----------------------------------------------------------------------------
// Helpers — path normalisation
// ----------------------------------------------------------------------------

// Normalise to absolute canonical path when possible; fall back to absolute
// path for paths that do not yet exist (e.g. watching a file before it is created).
static std::filesystem::path normalisePath(const std::filesystem::path &p)
{
    std::error_code ec;
    auto canonical = std::filesystem::canonical(p, ec);
    if (!ec)
        return canonical;
    return std::filesystem::weakly_canonical(p);
}

static std::chrono::milliseconds clampDebounce(std::chrono::milliseconds debounce)
{
    return debounce < std::chrono::milliseconds{0}
               ? std::chrono::milliseconds{0}
               : debounce;
}

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

// Default-construct with the built-in debounce value.
FileWatcher::FileWatcher() = default;

// Store the provided debounce interval; clamp negative values to zero.
FileWatcher::FileWatcher(std::chrono::milliseconds debounce)
    : m_debounce(clampDebounce(debounce))
{
}

// ----------------------------------------------------------------------------
// Registration
// ----------------------------------------------------------------------------

// Record the file path and snapshot its current write timestamp so poll()
// can detect future changes by comparing against it.
// NOTE: if the file does not yet exist, it is registered without a timestamp;
// poll() will fire Added when it first appears. Removed will not fire for a
// file that never existed before the first poll().
void FileWatcher::watchFile(const std::filesystem::path &filePath)
{
    const auto normalised = normalisePath(filePath);

    for (const auto &entry : m_entries)
        if (entry.path == normalised)
            return;

    WatchEntry entry;
    entry.path = normalised;
    entry.isDirectory = false;
    snapshotFile(entry);
    m_entries.push_back(std::move(entry));
}

// Record the directory path and snapshot write timestamps of all files inside it.
// When recursive is true, descend into subdirectories as well.
void FileWatcher::watchDirectory(const std::filesystem::path &directoryPath, bool recursive)
{
    const auto normalised = normalisePath(directoryPath);

    for (const auto &entry : m_entries)
        if (entry.path == normalised)
            return;

    WatchEntry entry;
    entry.path = normalised;
    entry.isDirectory = true;
    entry.recursive = recursive;
    snapshotDirectory(entry);
    m_entries.push_back(std::move(entry));
}

// Remove the entry matching path from the watch list so it is no longer polled.
void FileWatcher::unwatch(const std::filesystem::path &path)
{
    const auto normalised = normalisePath(path);
    m_entries.erase(
        std::remove_if(m_entries.begin(), m_entries.end(),
                       [&normalised](const WatchEntry &e)
                       { return e.path == normalised; }),
        m_entries.end());
}

// ----------------------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------------------

// Store the callback that will be invoked when poll() detects a stable change.
void FileWatcher::setChangeCallback(ChangeCallback callback)
{
    m_changeCallback = std::move(callback);
}

// Update the minimum quiet period required before a change event is emitted.
void FileWatcher::setDebounce(std::chrono::milliseconds debounce)
{
    m_debounce = clampDebounce(debounce);
}

// Return the current debounce interval.
std::chrono::milliseconds FileWatcher::getDebounce() const
{
    return m_debounce;
}

// ----------------------------------------------------------------------------
// Polling
// ----------------------------------------------------------------------------

// Iterate all watched paths and collect changes safely.
void FileWatcher::poll()
{
    const auto now = std::chrono::steady_clock::now();

    std::vector<PendingEvent> pendingEvents;
    pendingEvents.reserve(32);

    for (auto &entry : m_entries)
    {
        if (entry.isDirectory)
            pollDirectoryEntry(entry, now, pendingEvents);
        else
            pollFileEntry(entry, now, pendingEvents);
    }

    if (m_changeCallback)
    {
        for (const auto &e : pendingEvents)
            m_changeCallback(e.path, e.type);
    }
}

// Remove all watch entries and reset internal state.
void FileWatcher::clear()
{
    m_entries.clear();
}

// ----------------------------------------------------------------------------
// Snapshot helpers
// ----------------------------------------------------------------------------

void FileWatcher::snapshotFile(WatchEntry &entry) const
{
    std::error_code ec;
    const auto time = std::filesystem::last_write_time(entry.path, ec);
    if (!ec)
        entry.fileTimestamps[entry.path] = time;
}

void FileWatcher::snapshotDirectory(WatchEntry &entry) const
{
    std::error_code ec;

    auto snapshot = [&](auto &iter)
    {
        for (const auto &de : iter)
        {
            if (de.is_regular_file(ec))
            {
                const auto time = de.last_write_time(ec);
                if (!ec)
                    entry.fileTimestamps[de.path()] = time;
            }
        }
    };

    if (entry.recursive)
    {
        auto iter = std::filesystem::recursive_directory_iterator(entry.path, ec);
        if (!ec)
            snapshot(iter);
    }
    else
    {
        auto iter = std::filesystem::directory_iterator(entry.path, ec);
        if (!ec)
            snapshot(iter);
    }
}

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

bool FileWatcher::isDebounceReady(
    std::chrono::steady_clock::time_point lastNotification,
    std::chrono::steady_clock::time_point now) const
{
    return (now - lastNotification) >= m_debounce;
}

// ----------------------------------------------------------------------------
// File polling
// ----------------------------------------------------------------------------

void FileWatcher::pollFileEntry(
    WatchEntry &entry,
    std::chrono::steady_clock::time_point now,
    std::vector<PendingEvent> &pendingEvents)
{
    std::error_code ec;
    const bool exists = std::filesystem::exists(entry.path, ec);

    const auto trackedIt = entry.fileTimestamps.find(entry.path);
    const bool wasTracked = trackedIt != entry.fileTimestamps.end();

    if (!exists && wasTracked)
    {
        if (isDebounceReady(entry.lastNotification, now))
        {
            entry.fileTimestamps.erase(entry.path);
            entry.lastNotification = now;
            pendingEvents.push_back({entry.path, ChangeType::Removed});
        }
        return;
    }

    if (exists && !wasTracked)
    {
        if (isDebounceReady(entry.lastNotification, now))
        {
            const auto time = std::filesystem::last_write_time(entry.path, ec);
            if (!ec)
            {
                entry.fileTimestamps[entry.path] = time;
                entry.lastNotification = now;
                pendingEvents.push_back({entry.path, ChangeType::Added});
            }
        }
        return;
    }

    if (exists && wasTracked)
    {
        const auto currentTime = std::filesystem::last_write_time(entry.path, ec);
        if (!ec && currentTime != trackedIt->second)
        {
            if (isDebounceReady(entry.lastNotification, now))
            {
                trackedIt->second = currentTime;
                entry.lastNotification = now;
                pendingEvents.push_back({entry.path, ChangeType::Modified});
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Directory polling
// ----------------------------------------------------------------------------

void FileWatcher::pollDirectoryEntry(
    WatchEntry &entry,
    std::chrono::steady_clock::time_point now,
    std::vector<PendingEvent> &pendingEvents)
{
    std::error_code ec;

    if (!std::filesystem::exists(entry.path, ec) || ec)
    {
        for (auto it = entry.fileTimestamps.begin(); it != entry.fileTimestamps.end();)
        {
            auto &lastNotif = entry.fileLastNotifications[it->first];
            if (isDebounceReady(lastNotif, now))
            {
                pendingEvents.push_back({it->first, ChangeType::Removed});
                entry.fileLastNotifications.erase(it->first);
                it = entry.fileTimestamps.erase(it);
            }
            else
                ++it;
        }
        return;
    }

    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> current;

    auto collect = [&](auto &iter)
    {
        for (const auto &de : iter)
        {
            if (de.is_regular_file(ec))
            {
                const auto time = de.last_write_time(ec);
                if (!ec)
                    current[de.path()] = time;
            }
        }
    };

    if (entry.recursive)
    {
        auto iter = std::filesystem::recursive_directory_iterator(entry.path, ec);
        if (!ec)
            collect(iter);
    }
    else
    {
        auto iter = std::filesystem::directory_iterator(entry.path, ec);
        if (!ec)
            collect(iter);
    }

    for (const auto &[path, currentTime] : current)
    {
        auto &lastNotif = entry.fileLastNotifications[path];
        const auto it = entry.fileTimestamps.find(path);

        if (it == entry.fileTimestamps.end())
        {
            if (isDebounceReady(lastNotif, now))
            {
                entry.fileTimestamps[path] = currentTime;
                lastNotif = now;
                pendingEvents.push_back({path, ChangeType::Added});
            }
        }
        else if (it->second != currentTime)
        {
            if (isDebounceReady(lastNotif, now))
            {
                it->second = currentTime;
                lastNotif = now;
                pendingEvents.push_back({path, ChangeType::Modified});
            }
        }
    }

    for (auto it = entry.fileTimestamps.begin(); it != entry.fileTimestamps.end();)
    {
        if (current.count(it->first) == 0)
        {
            auto &lastNotif = entry.fileLastNotifications[it->first];
            if (isDebounceReady(lastNotif, now))
            {
                pendingEvents.push_back({it->first, ChangeType::Removed});
                entry.fileLastNotifications.erase(it->first);
                it = entry.fileTimestamps.erase(it);
            }
            else
                ++it;
        }
        else
            ++it;
    }
}