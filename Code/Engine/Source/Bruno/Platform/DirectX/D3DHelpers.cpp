#include "brpch.h"
#include "D3DHelpers.h"

#include "GraphicsDevice.h"
#include "Berta/Core/Base.h"
#include "Berta/Core/Log.h"

namespace Bruno
{
    void ThrowIfFailed(HRESULT hr)
    {
#if BT_DEBUG
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_HUNG)
        {
            auto nativeDevice = Graphics::GetDevice()->GetNativeDevice();
            Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedData> dred;
            if (SUCCEEDED(nativeDevice->QueryInterface(IID_PPV_ARGS(&dred))))
            {
                D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT breadcrumbs = {};
                if (SUCCEEDED(dred->GetAutoBreadcrumbsOutput(&breadcrumbs)))
                {
                    for (const auto* node = breadcrumbs.pHeadAutoBreadcrumbNode; node != nullptr; node = node->pNext)
                    {
                        UINT32 lastOp = node->pLastBreadcrumbValue ? *node->pLastBreadcrumbValue : 0;
                        BR_CORE_TRACE << "[DRED] CmdList: " << Berta::StringUtils::WideToUTF8(node->pCommandListDebugNameW)
                                  << " | Ultimo comando ejecutado: " << lastOp << "/" << node->BreadcrumbCount << "\n";

                        UINT32 start = (lastOp > 5) ? (lastOp - 5) : 0;
                        UINT32 end   = std::min<uint32_t>(node->BreadcrumbCount, lastOp + 3);
                        for (UINT32 i = start; i < end; ++i)
                        {
                            BR_CORE_TRACE << "   Op[" << i << "] = " << static_cast<int>(node->pCommandHistory[i]) << "\n";
                        }
                    }
                }

                D3D12_DRED_PAGE_FAULT_OUTPUT pageFault = {};
                if (SUCCEEDED(dred->GetPageFaultAllocationOutput(&pageFault)) && pageFault.PageFaultVA != 0)
                {
                    BR_CORE_TRACE << "[DRED] PageFaultVA: 0x"
                              << std::hex << std::uppercase << std::setw(16) << std::setfill('0')
                              << pageFault.PageFaultVA
                              << std::dec << std::setfill(' ') << "\n";

                    for (auto* n = pageFault.pHeadExistingAllocationNode; n != nullptr; n = n->pNext)
                    {
                        if (n->ObjectNameW)
                        {
                            BR_CORE_TRACE << "   [Existente] " << Berta::StringUtils::WideToUTF8(n->ObjectNameW) << "\n";
                        }
                    }

                    for (auto* n = pageFault.pHeadRecentFreedAllocationNode; n != nullptr; n = n->pNext)
                    {
                        if (n->ObjectNameW)
                        {
                            BR_CORE_TRACE<< "   [Liberado]  " << Berta::StringUtils::WideToUTF8(n->ObjectNameW) << "\n";
                        }
                    }
                }

                BR_CORE_TRACE << std::endl;
            }
        }
#endif
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}
