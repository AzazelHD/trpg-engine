#pragma once
// FileWatcher monitors registered files and directories for filesystem changes
// using polling and write-time comparison.
// Designed to feed the hot-reload pipeline during development; not used in release builds.
// All methods are non-thread-safe; call from a single thread (typically the main/game thread).
#include <chrono>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

class FileWatcher
{
public:
    enum class ChangeType
    {
        Added,
        Modified,
        Removed
    };

    using ChangeCallback =
        std::function<void(const std::filesystem::path &changedPath, ChangeType changeType)>;

    FileWatcher();
    explicit FileWatcher(std::chrono::milliseconds debounce);

    void watchFile(const std::filesystem::path &filePath);
    void watchDirectory(const std::filesystem::path &directoryPath, bool recursive = false);
    void unwatch(const std::filesystem::path &path);

    void setChangeCallback(ChangeCallback callback);
    void setDebounce(std::chrono::milliseconds debounce);
    [[nodiscard]] std::chrono::milliseconds getDebounce() const;

    void poll();
    void clear();

private:
    struct PendingEvent
    {
        std::filesystem::path path;
        ChangeType type;
    };
    struct WatchEntry
    {
        std::filesystem::path path;
        bool isDirectory = false;
        bool recursive = false;
        std::chrono::steady_clock::time_point lastNotification{};
        std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> fileTimestamps;
        std::unordered_map<std::filesystem::path, std::chrono::steady_clock::time_point> fileLastNotifications;
    };

    // Snapshot helpers — populate fileTimestamps from disk.
    void snapshotFile(WatchEntry &entry) const;
    void snapshotDirectory(WatchEntry &entry) const;

    // Poll helpers — compare current disk state against snapshots and fire callback.
    void pollFileEntry(
        WatchEntry &entry,
        std::chrono::steady_clock::time_point now,
        std::vector<PendingEvent> &pendingEvents);

    void pollDirectoryEntry(
        WatchEntry &entry,
        std::chrono::steady_clock::time_point now,
        std::vector<PendingEvent> &pendingEvents);

    // Returns true if enough time has elapsed since lastNotification to satisfy the debounce.
    [[nodiscard]] bool isDebounceReady(
        std::chrono::steady_clock::time_point lastNotification,
        std::chrono::steady_clock::time_point now) const;

    std::chrono::milliseconds m_debounce{100};
    ChangeCallback m_changeCallback;
    std::vector<WatchEntry> m_entries;
};