
#include "engine/core/Timer.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>

// [x]: Implement start().
//   Call QueryPerformanceFrequency to get m_frequency.
//   Call QueryPerformanceCounter to capture m_startCounter and m_lastCounter.
void Timer::start()
{
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    m_frequency = freq.QuadPart;
    m_startCounter = counter.QuadPart;
    m_lastCounter = counter.QuadPart;
}

// [x]: Implement tick().
//   Call QueryPerformanceCounter to get the current counter.
//   delta = (current - m_last) / m_frequency   (gives seconds as double, cast to float)
//   Update m_last = current.
//   Return min(delta, 0.05f)  ← cap to prevent spiral of death.
float Timer::tick()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float delta = static_cast<float>(now.QuadPart - m_lastCounter) / static_cast<float>(m_frequency);
    m_lastCounter = now.QuadPart;
    // Cap delta to 0.05s (avoid spiral of death)
    return std::min(delta, 0.05f);
}

// [x]: Implement getTotalTime().
//   (currentCounter - m_startCounter) / m_frequency
float Timer::getTotalTime() const
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<float>(now.QuadPart - m_startCounter) / static_cast<float>(m_frequency);
}