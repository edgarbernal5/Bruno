#include "brpch.h"
#include "SwapChain.h"
#include "GraphicsDevice.h"
#include "CommandQueue.h"
#include "Texture2D.h"

namespace Bruno
{
    SwapChain::SwapChain(GraphicsDevice& device, SurfaceWindowParameters const& parameters)
        : m_device(device), m_currentBufferIndex(0), m_parameters(parameters)
    {
        HWND hwnd = parameters.WindowHandle;
        auto nativeDevice = m_device.GetNativeDevice();
        auto dxgiFactory = m_device.GetDXGIFactory();
        auto commandQueue = m_device.GetDirectCommandQueue().GetNativeQueue();

        // 1. Describir cómo queremos el Swap Chain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = parameters.Width;
        swapChainDesc.Height = parameters.Height;
        swapChainDesc.Format = parameters.BackBufferFormat;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 }; // DX12 requiere que el SwapChain no tenga MSAA (se hace en otro lado)
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = Graphics::Core::BACK_BUFFER_COUNT; // Doble buffering (2)
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        
        // El estándar de oro moderno para evitar lag de input y tearing
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        
        // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING si quisieras FPS ilimitados (G-Sync/FreeSync)
        swapChainDesc.Flags = 0;

        // Crear el SwapChain original
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
        ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            commandQueue.Get(), // DX12 exige que le pases la cola de comandos aquí, no el Device
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        ));

        ThrowIfFailed(swapChain1.As(&m_swapChain));
        
        // Actualizar el índice del buffer actual (0 o 1)
        m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
        
        m_rtvAllocator = std::make_unique<DescriptorAllocator>(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, swapChainDesc.BufferCount, false);
        
        for (uint32_t i = 0; i < swapChainDesc.BufferCount; i++)
        {
            m_renderTargets[i].RtvHandle = m_rtvAllocator->Allocate(1);
        }
        
        // Vincular las texturas reales a los descriptores
        UpdateRenderTargetViews();
        
        // Prohibir a DXGI que intercepte Alt+Enter (Nosotros lo manejaremos si queremos fullscreen)
        ThrowIfFailed(dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
    }

    SwapChain::~SwapChain()
    {
        // En RAII, ComPtr se encarga de destruir las texturas y el SwapChain.
        // Solo asegúrate de que el GraphicsDevice haga un Flush() antes de llegar aquí.
        m_device.GetDirectCommandQueue().Flush();
    }

    void SwapChain::UpdateRenderTargetViews()
    {
        auto nativeDevice = m_device.GetNativeDevice();
        
        for (uint32_t i = 0; i < Graphics::Core::BACK_BUFFER_COUNT; i++)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource;
            
            // Extraer la textura 2D del BackBuffer directamente del SwapChain
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource)));
            
            if (!m_renderTargets[i].Resource)
            {
                m_renderTargets[i].Resource = std::make_shared<Texture2D>();
            }
            
            // Enchufar la textura en el RTV Heap de DirectX 12
            nativeDevice->CreateRenderTargetView(backBufferResource.Get(), nullptr, m_renderTargets[i].RtvHandle.CPU);
            m_renderTargets[i].Resource->AttachNativeResource(backBufferResource, m_renderTargets[i].RtvHandle);
        }
    }

    void SwapChain::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            return; 
        }
        
        // REGLA DE ORO: No puedes redimensionar algo que la GPU está usando.
        // Llamamos al método Flush que hicimos en el paso anterior.
        m_device.GetDirectCommandQueue().Flush();

        // 1. Liberar los punteros de las texturas viejas (si no, DX12 tirará un error al redimensionar)
        for (uint32_t i = 0; i < Graphics::Core::BACK_BUFFER_COUNT; i++)
        {
            m_renderTargets[i].Resource.reset();
        }

        // 2. Redimensionar el SwapChain
        ThrowIfFailed(m_swapChain->ResizeBuffers(
            Graphics::Core::BACK_BUFFER_COUNT, width, height,
            m_parameters.BackBufferFormat, 0
        ));

        // 3. Sincronizar nuestro índice
        m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        // 4. Volver a extraer las nuevas texturas redimensionadas
        UpdateRenderTargetViews();
    }

    void SwapChain::Present(bool vsync)
    {
        UINT syncInterval = vsync ? 1 : 0;
        UINT presentFlags = 0;
        
        // Si quitamos el VSync y el SwapChain lo soporta, le decimos a DX12 que 
        // permita "Tearing" para no limitar los FPS al monitor.
        // (Nota: Para que ALLOW_TEARING funcione del todo, deberías haberle pasado el flag
        // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING al swapChainDesc en el constructor).
        if (!vsync)
        {
            presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        }
        ThrowIfFailed(m_swapChain->Present(syncInterval, presentFlags));
        
        // Actualizar cuál es el buffer en el que vamos a dibujar en el siguiente frame
        m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    uint32_t SwapChain::GetCurrentBackBufferIndex() const
    {
        return m_currentBufferIndex;
    }

    Texture2D* SwapChain::GetCurrentRenderTarget() const
    {
        return m_renderTargets[m_currentBufferIndex].Resource.get();
    }
}
