#pragma once

#include <wrl/client.h>
#include <chrono>
#include <cstdint>

namespace Bruno
{
    class GraphicsDevice;
    
    struct FrameStatistics
    {
        // Tiempos
        float CpuCullingTimeMs = 0.0f;
        float CpuTotalRenderTimeMs = 0.0f;
        float GpuRenderTimeMs = 0.0f;
    
        // Geometría y Culling
        uint32_t TotalEntities = 0;
        uint32_t RenderedEntities = 0;
        uint32_t DrawCalls = 0;
        uint32_t TriangleCount = 0;

        void ResetCounters()
        {
            TotalEntities = 0;
            RenderedEntities = 0;
            DrawCalls = 0;
            TriangleCount = 0;
        }
    };
    
    class Profiler
    {
    public:
        FrameStatistics Stats;

        // Inicializa los recursos de DX12 para medir tiempos
        void Initialize(GraphicsDevice* device, ID3D12CommandQueue* commandQueue);

        // Métodos para la GPU
        void StartGpuTimer(ID3D12GraphicsCommandList* cmdList);
        void StopGpuTimer(ID3D12GraphicsCommandList* cmdList);
        void ResolveGpuTimestamps(ID3D12GraphicsCommandList* cmdList);
        void ReadbackGpuTimes(); // Se llama una vez que la GPU terminó el frame

        // Singleton o instancia global inyectada
        static Profiler& Get()
        {
            static Profiler instance;
            return instance;
        }
    private:
        Profiler() = default;

        Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_queryHeap;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_readbackBuffer;
    
        uint64_t m_gpuTickFrequency = 0; // Cuántos ticks de GPU hay en 1 segundo
    };
}
