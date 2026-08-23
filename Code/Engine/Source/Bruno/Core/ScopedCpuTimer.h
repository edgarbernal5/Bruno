#pragma once

#include <chrono>

namespace Bruno
{
    class ScopedCpuTimer
    {
    public:
        ScopedCpuTimer(float* targetVariable);
        ~ScopedCpuTimer();

    private:
        float* m_targetVariable;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}