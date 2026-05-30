#include "brpch.h"
#include "SwapChain.h"
#include "Device.h"

namespace Bruno::DX
{
    SwapChain::SwapChain(GraphicsDevice& device, void* windowHandle, uint32_t width, uint32_t height)
        : m_device(device)
    {
        CreateSwapChain(windowHandle, width, height);
        CreateRenderTargets();
    }

    void SwapChain::Present()
    {
        // 1 para VSync (sincronizar con el refresco del monitor), 0 para FPS ilimitados
        HRESULT hr = m_swapChain->Present(1, 0);
        if (FAILED(hr))
        {
            // Aquí podrías integrar tu sistema de logs o aserciones de Bruno
            throw std::runtime_error("Fallo al hacer Present en el SwapChain.");
        }

        // Actualizamos el índice al siguiente buffer en el anillo (Ring Buffer)
        m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChain::Resize(uint32_t width, uint32_t height)
    {
        // 1. IMPORTANTE: Antes de redimensionar, debemos liberar las referencias actuales
        // de los buffers, de lo contrario ResizeBuffers fallará.
        for (uint32_t i = 0; i < FrameCount; ++i)
        {
            m_renderTargets[i].Reset();
        }

        // 2. Redimensionar los buffers internos
        HRESULT hr = m_swapChain->ResizeBuffers(
            FrameCount,
            width,
            height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            0 // Puedes usar DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING aquí si soportas VRR/G-Sync
        );

        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al redimensionar el SwapChain.");
        }

        // 3. Reiniciar el índice actual y recrear las vistas (RTVs)
        m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
        CreateRenderTargets();
    }

    uint32_t SwapChain::GetCurrentBackBufferIndex() const
    {
        return m_currentFrameIndex;
    }

    ID3D12Resource* SwapChain::GetCurrentBackBuffer() const
    {
        return m_renderTargets[m_currentFrameIndex].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentBackBufferView() const
    {
        // Magia de C++ y DX12: Calculamos la dirección de memoria exacta de nuestro descriptor
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += static_cast<SIZE_T>(m_currentFrameIndex) * m_rtvDescriptorSize;
        return rtvHandle;
    }

    void SwapChain::CreateSwapChain(void* windowHandle, uint32_t width, uint32_t height)
    {
        // Estructura de configuración del SwapChain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Obligatorio para DX12 (El modelo moderno)
        swapChainDesc.SampleDesc.Count = 1; // DX12 no soporta MSAA directo en el SwapChain, debe resolverse antes
        swapChainDesc.SampleDesc.Quality = 0;

        // DXGI necesita el CommandQueue principal para saber cuándo la GPU terminó de dibujar
        auto commandQueue = m_device.GetCommandQueue();
        auto dxgiFactory = m_device.GetFactory();

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
            commandQueue,
            static_cast<HWND>(windowHandle),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        );

        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al crear la estructura SwapChain para HWND.");
        }

        // DXGI no permite usar Alt+Enter por defecto, es mejor manejar el fullscreen a mano en tu motor
        dxgiFactory->MakeWindowAssociation(static_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER);

        // Hacemos el cast seguro a IDXGISwapChain3 (que nos da GetCurrentBackBufferIndex)
        hr = swapChain1.As(&m_swapChain);
        if (FAILED(hr))
        {
            throw std::runtime_error("La versión de DirectX no soporta IDXGISwapChain3.");
        }

        m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChain::CreateRenderTargets()
    {
        auto d3dDevice = m_device.GetNativeDevice();

        // 1. Crear el Descriptor Heap para los Render Target Views (RTVs)
        // (Si ya existía, lo creará de nuevo limpio, lo cual es correcto tras un Resize)
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
        if (FAILED(hr))
        {
            throw std::runtime_error("Fallo al crear el RTV Descriptor Heap.");
        }

        // El tamaño de los descriptores puede variar según la GPU, debemos preguntarle a la tarjeta gráfica
        m_rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // 2. Extraer los buffers (Texturas 2D) del SwapChain y crear sus RTVs
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        for (uint32_t i = 0; i < FrameCount; ++i)
        {
            hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            if (FAILED(hr))
            {
                throw std::runtime_error("Fallo al obtener el BackBuffer del SwapChain.");
            }

            d3dDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            
            // Avanzamos el puntero al siguiente descriptor
            rtvHandle.ptr += m_rtvDescriptorSize;
        }
    }
}
