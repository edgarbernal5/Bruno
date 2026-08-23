#include "brpch.h"
#include "ScopedCpuTimer.h"

namespace Bruno
{
    ScopedCpuTimer::ScopedCpuTimer(float* targetVariable) : 
        m_targetVariable(targetVariable)
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    ScopedCpuTimer::~ScopedCpuTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> duration = end - m_start;
        if (m_targetVariable)
        {
            *m_targetVariable = duration.count();
        }
    }
}
