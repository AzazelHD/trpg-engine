#pragma once
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

// Simple tagged logger. Wraps printf/OutputDebugString so we can strip it in Release.
//
// [x]: Implement as a set of macros (not a class) so the compiler can remove them entirely
// in Release builds using #ifdef _DEBUG.
//
// Suggested macros:
//   LOG_INFO(tag, fmt, ...)   — general info
//   LOG_WARN(tag, fmt, ...)   — something unexpected but non-fatal
//   LOG_ERROR(tag, fmt, ...)  — serious problem, won't crash but should be investigated
//
// Example output format: "[INFO][Renderer] Texture loaded: hero.png"
//
// In Release: all macros expand to nothing (zero overhead).
// Tip: use __VA_ARGS__ and look up how to write variadic macros in C++.

#ifdef _DEBUG
namespace engine_log
{
    inline unsigned long long &currentFrame()
    {
        static unsigned long long frame = 0;
        return frame;
    }

    inline void setFrame(unsigned long long frame)
    {
        currentFrame() = frame;
    }

    inline void buildTimestamp(char *out, std::size_t outSize)
    {
        using namespace std::chrono;

        const auto now = system_clock::now();
        const auto nowTimeT = system_clock::to_time_t(now);
        std::tm localTm{};
        (void)localtime_s(&localTm, &nowTimeT);

        char dateTime[32] = {};
        (void)std::strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", &localTm);

        const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        (void)std::snprintf(out, outSize, "%s.%03lld", dateTime, static_cast<long long>(ms.count()));
    }

    inline std::FILE *latestRunFile()
    {
        static std::FILE *file = []() -> std::FILE *
        {
            std::FILE *opened = nullptr;
            (void)::fopen_s(&opened, "latest_debug_run.txt", "w");
            return opened;
        }();
        return file;
    }

    inline void write(const char *level, const char *tag, const char *fmt, ...)
    {
        char timestamp[48] = {};
        buildTimestamp(timestamp, sizeof(timestamp));

        char payload[1024] = {};
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(payload, sizeof(payload), fmt, args);
        va_end(args);

        char line[1400] = {};
        std::snprintf(
            line,
            sizeof(line),
            "[%s][F:%llu][%s][%s] %s\n",
            timestamp,
            currentFrame(),
            level,
            tag,
            payload);

        std::fputs(line, stdout);
        std::fflush(stdout);

        if (std::FILE *file = latestRunFile())
        {
            std::fputs(line, file);
            std::fflush(file);
        }

#ifdef _WIN32
        OutputDebugStringA(line);
#endif
    }
}

#define LOG_SET_FRAME(frame) engine_log::setFrame(static_cast<unsigned long long>(frame))
#define LOG_INFO(tag, fmt, ...) engine_log::write("INFO", tag, fmt, ##__VA_ARGS__)
#define LOG_WARN(tag, fmt, ...) engine_log::write("WARN", tag, fmt, ##__VA_ARGS__)
#define LOG_ERROR(tag, fmt, ...) engine_log::write("ERROR", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_SET_FRAME(frame) ((void)0)
#define LOG_INFO(tag, fmt, ...) ((void)0)
#define LOG_WARN(tag, fmt, ...) ((void)0)
#define LOG_ERROR(tag, fmt, ...) ((void)0)
#endif
