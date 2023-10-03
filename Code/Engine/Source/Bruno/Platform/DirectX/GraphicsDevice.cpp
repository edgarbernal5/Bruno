#include "brpch.h"
#include "GraphicsDevice.h"

#include "GraphicsAdapter.h"
#include "CommandQueue.h"
#include "UploadCommand.h"

#include "D3D12MemAlloc.h"

#include <numeric>

namespace Bruno
{
	GraphicsDevice::GraphicsDevice(std::shared_ptr<GraphicsAdapter> adapter) :
		m_adapter(adapter)
	{
        if (!m_adapter)
        {
            m_adapter = GraphicsAdapter::Create();
        }

#if BR_DEBUG
        Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
        {
            m_dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

            DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
            {
                80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
            };
            DXGI_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<uint32_t>(std::size(hide));
            filter.DenyList.pIDList = hide;
            dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
        }
        
#endif
        ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

        // Determines whether tearing support is available for fullscreen borderless windows.
        //if (m_options & c_AllowTearing)
        //{
        //}

        // Create the DX12 API device object.
        HRESULT hr = D3D12CreateDevice(
            m_adapter->GetHandle(),
            m_d3dMinFeatureLevel,
            IID_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf())
        );
        ThrowIfFailed(hr);

        m_d3dDevice->SetName(L"GraphicsDevice");

#ifndef NDEBUG
        // Configure debug device (if active).
        Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D12_MESSAGE_ID hide[] =
            {
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                // Workarounds for debug layer issues on hybrid-graphics systems
                D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
            };
            D3D12_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<uint32_t>(std::size(hide));
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
#endif

        // Determine maximum supported feature level for this device
        static const D3D_FEATURE_LEVEL s_featureLevels[] =
        {
    #if defined(NTDDI_WIN10_FE) || defined(USING_D3D12_AGILITY_SDK)
            D3D_FEATURE_LEVEL_12_2,
    #endif
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
        {
            static_cast<uint32_t>(std::size(s_featureLevels)), s_featureLevels, D3D_FEATURE_LEVEL_11_0
        };

        hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
        if (SUCCEEDED(hr))
        {
            m_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;
        }
        else
        {
            m_d3dFeatureLevel = m_d3dMinFeatureLevel;
        }

        m_commandQueue = std::make_unique<CommandQueue>(this, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_uploadCommand = std::make_unique<UploadCommand>(this);

        m_rtvDescriptorHeap = std::make_unique<StagingDescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, Graphics::Core::NUM_RTV_STAGING_DESCRIPTORS);
        m_dsvDescriptorHeap = std::make_unique<StagingDescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, Graphics::Core::NUM_DSV_STAGING_DESCRIPTORS);
        m_srvDescriptorHeap = std::make_unique<StagingDescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Graphics::Core::NUM_SRV_STAGING_DESCRIPTORS);
        mSamplerRenderPassDescriptorHeap = std::make_unique<RenderPassDescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 0, Graphics::Core::NUM_SAMPLER_DESCRIPTORS);

        for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
        {
            mSRVRenderPassDescriptorHeaps[frameIndex] = std::make_unique<RenderPassDescriptorHeap>(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Graphics::Core::NUM_RESERVED_SRV_DESCRIPTORS, Graphics::Core::NUM_SRV_RENDER_PASS_USER_DESCRIPTORS);
        }

        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If
        // CheckFeatureSupport succeeds, the HighestVersion returned will not be
        // greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
            &featureData,
            sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        m_highestRootSignatureVersion = featureData.HighestVersion;

        D3D12MA::ALLOCATOR_DESC desc = {};
        desc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        desc.pDevice = m_d3dDevice.Get();
        desc.pAdapter = m_adapter.get()->GetHandle();

        D3D12MA::CreateAllocator(&desc, &mAllocator);

        mFreeReservedDescriptorIndices.resize(Graphics::Core::NUM_RESERVED_SRV_DESCRIPTORS);
        for (size_t i = 0; i < mFreeReservedDescriptorIndices.size(); i++)
        {
            mFreeReservedDescriptorIndices[i] = i;
        }
	}

    void GraphicsDevice::BeginFrame()
    {
        m_frameId = (m_frameId + 1) % Graphics::Core::FRAMES_IN_FLIGHT_COUNT;


    }

    void GraphicsDevice::EndFrame()
    {
    }

    D3D12MA::Allocator* GraphicsDevice::GetAllocator() const
    {
        return mAllocator;
    }

    uint32_t GraphicsDevice::GetFreeReservedDescriptorIndex()
    {
        uint32_t index = mFreeReservedDescriptorIndices.back();
        mFreeReservedDescriptorIndices.pop_back();
        return index;
    }

    std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(std::shared_ptr<GraphicsAdapter> adapter)
    {
        return std::make_shared<GraphicsDevice>(adapter);
    }

    void GraphicsDevice::CopySRVHandleToReservedTable(DescriptorHandle srvHandle, uint32_t index)
    {
        for (uint32_t frameIndex = 0; frameIndex < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; frameIndex++)
        {
            DescriptorHandle targetDescriptor = mSRVRenderPassDescriptorHeaps[frameIndex]->GetReservedDescriptor(index);

            //CopyDescriptorsSimple(1, targetDescriptor.mCPUHandle, srvHandle.mCPUHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            m_d3dDevice->CopyDescriptorsSimple(1, targetDescriptor.Cpu, srvHandle.Cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }
    }

    IDXGIFactory4* GraphicsDevice::GetFactory()
    {
        return m_dxgiFactory.Get();
    }

    ID3D12Device5* GraphicsDevice::GetD3DDevice()
    {
        return m_d3dDevice.Get();
    }

    CommandQueue* GraphicsDevice::GetCommandQueue()
    {
        return m_commandQueue.get();
    }

    UploadCommand* GraphicsDevice::GetUploadCommand()
    {
        return m_uploadCommand.get();
    }

    StagingDescriptorHeap& GraphicsDevice::GetRtvDescriptionHeap()
    {
        return *m_rtvDescriptorHeap;
    }

    StagingDescriptorHeap& GraphicsDevice::GetDsvDescriptionHeap()
    {
        return *m_dsvDescriptorHeap;
    }

    StagingDescriptorHeap& GraphicsDevice::GetSrvDescriptionHeap()
    {
        return *m_srvDescriptorHeap;
    }
}