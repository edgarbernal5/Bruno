#include "brpch.h"
#include "Device.h"

#ifdef BR_DEBUG
#include <dxgidebug.h>
#endif

#include "D3DHelpers.h"

#include <numeric>

//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION; }
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

namespace Bruno::DX
{
    GraphicsDevice::GraphicsDevice() 
    {
        EnableDebugLayer();
        CreateDevice();
        CreateCommandQueue();
    }

    void GraphicsDevice::EnableDebugLayer() 
    {
#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) 
        {
            debugController->EnableDebugLayer();
        }
#endif
    }

    void GraphicsDevice::CreateDevice() 
    {
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));

        // KISS: Aquí podrías iterar sobre los adaptadores (tarjetas gráficas)
        // Por ahora, le pasamos nullptr para usar la GPU por defecto.
        ThrowIfFailed(D3D12CreateDevice(
            nullptr, 
            D3D_FEATURE_LEVEL_12_0, 
            IID_PPV_ARGS(&m_device)
        ));
    }

    void GraphicsDevice::CreateCommandQueue() 
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
    }
}