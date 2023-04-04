#include "brpch.h"
#include "D3DCore.h"

namespace Bruno::Device::Core
{
    void EnableDebugLayer()
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
        {
            debugInterface->EnableDebugLayer();
        }
        else
        {
            std::cout << "Warning: D3D12 Debug interface is not available." << std::endl;
        }
    }
}