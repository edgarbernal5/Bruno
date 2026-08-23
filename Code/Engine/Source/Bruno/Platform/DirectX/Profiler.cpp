#include "brpch.h"
#include "Profiler.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    void Profiler::Initialize(GraphicsDevice* device, ID3D12CommandQueue* commandQueue)
    {
        auto nativeDevice = device->GetNativeDevice();
        
        // 1. Obtener la frecuencia de la GPU (vital para convertir Ticks a Milisegundos)
        commandQueue->GetTimestampFrequency(&m_gpuTickFrequency);

        // 2. Crear el Query Heap (Necesitamos 2 espacios: Inicio y Fin)
        D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
        queryHeapDesc.Count = 2; 
        queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
        nativeDevice->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_queryHeap));

        // 3. Crear el Readback Buffer (Para leer los 2 uint64_t que devuelve la GPU)
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_READBACK;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = sizeof(uint64_t) * 2;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        nativeDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_readbackBuffer)
        );
    }

    void Profiler::StartGpuTimer(ID3D12GraphicsCommandList* cmdList)
    {
        // Inserta el comando de marca de tiempo en el índice 0
        cmdList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0);
    }

    void Profiler::StopGpuTimer(ID3D12GraphicsCommandList* cmdList)
    {
        // Inserta el comando de marca de tiempo en el índice 1
        cmdList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 1);
    }

    void Profiler::ResolveGpuTimestamps(ID3D12GraphicsCommandList* cmdList)
    {
        // Le decimos a la GPU que copie los datos del QueryHeap al ReadbackBuffer
        cmdList->ResolveQueryData(
            m_queryHeap.Get(), 
            D3D12_QUERY_TYPE_TIMESTAMP, 
            0, 2, 
            m_readbackBuffer.Get(), 0
        );
    }

    void Profiler::ReadbackGpuTimes()
    {
        // Mapeamos la memoria de la GPU a la CPU para leer los resultados
        uint64_t* mappedData = nullptr;
        D3D12_RANGE readRange = { 0, sizeof(uint64_t) * 2 };
    
        if (SUCCEEDED(m_readbackBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedData))))
        {
            uint64_t startTime = mappedData[0];
            uint64_t endTime = mappedData[1];
            m_readbackBuffer->Unmap(0, nullptr);

            // Convertir los ticks a milisegundos reales
            if (endTime > startTime && m_gpuTickFrequency > 0)
            {
                uint64_t delta = endTime - startTime;
                Stats.GpuRenderTimeMs = static_cast<float>((static_cast<double>(delta) / m_gpuTickFrequency) * 1000.0);
            }
        }
    }
}
