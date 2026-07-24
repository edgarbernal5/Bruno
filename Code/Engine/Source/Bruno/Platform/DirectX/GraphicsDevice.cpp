#include "brpch.h"
#include "GraphicsDevice.h"

#ifdef BR_DEBUG
#include <dxgidebug.h>
#endif

#include "D3DHelpers.h"
#include "CommandQueue.h"
#include "UploadContext.h"

#include <numeric>

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

namespace Bruno
{
    GraphicsDevice::GraphicsDevice() 
    {
        InitializeDXGI();
        CreateDevice();
        
        // C++14/17: Creación segura de memoria dinámica (RAII)
        m_directCommandQueue = std::make_unique<CommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
        
        m_srvDescriptorAllocator = std::make_unique<DescriptorAllocator>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096, true);
        m_uploadContext = std::make_unique<UploadContext>(*this);
    }

    void GraphicsDevice::Flush()
    {
        GetDirectCommandQueue().Flush();
    }

    std::shared_ptr<GraphicsDevice> GraphicsDevice::Create()
    {
        return std::make_shared<GraphicsDevice>();
    }

    void GraphicsDevice::InitializeDXGI()
    {
        UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
        // Encender la capa de depuración de DX12. ¡Te salvará horas de dolores de cabeza!
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG; // Opcional en DXGI 1.3+
        }
#endif

        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
    }

    void GraphicsDevice::CreateDevice()
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        
        // Iteramos por las tarjetas gráficas de la PC para encontrar una compatible con DX12
        for (UINT adapterIndex = 0; 
             DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, &adapter); 
             ++adapterIndex) 
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Ignoramos el renderizador por software (Basic Render Driver)
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            // Verificamos si soporta DX12 (Feature Level 11_0 es el mínimo para DX12 API)
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                hardwareAdapter = adapter;
                break;
            }
        }

        if (!hardwareAdapter)
        {
            throw std::runtime_error("No se encontró una tarjeta gráfica compatible con DirectX 12.");
        }

        // Creamos el Device real
        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(), 
            D3D_FEATURE_LEVEL_11_0, 
            IID_PPV_ARGS(&m_device)
        ));
    }
}