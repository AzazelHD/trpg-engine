#pragma once
#include <cstdint>

// Timer measures elapsed time between frames for the main loop.
// Uses QueryPerformanceCounter on Windows for high-resolution timing.
//
// [x]: Declare the class with:
//   - start(): call once before the loop begins, captures the initial counter.
//   - tick(): call once per frame. Returns the delta time in seconds (float).
//             Internally: record now, compute delta = (now - last) / frequency, update last.
//   - getTotalTime(): total seconds since start() was called.
//
// Tip: cap the returned dt to ~0.05s (3 missed frames) to avoid the "spiral of death"
// when the game hangs or the debugger pauses execution.

class Timer
{
public:
    void start();
    // Call once before the loop begins, captures the initial counter.

    float tick();
    // Call once per frame. Returns the delta time in seconds (float).
    // Internally: record now, compute delta = (now - last) / frequency, update last.
    // Caps dt to ~0.05s to avoid spiral of death.

    float getTotalTime() const;
    // Returns total seconds since start() was called.

private:
    int64_t m_startCounter = 0;
    int64_t m_lastCounter = 0;
    int64_t m_frequency = 0;
};
