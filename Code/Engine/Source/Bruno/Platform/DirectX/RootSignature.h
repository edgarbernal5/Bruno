#pragma once

#include "D3DCommon.h"

namespace Bruno
{
    class GraphicsDevice;

	class RootSignature
	{
	public:
        RootSignature(GraphicsDevice* device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);
        ~RootSignature();

        ID3D12RootSignature* GetD3D12RootSignature() const
        {
            return m_rootSignature.Get();
        }

        const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const
        {
            return m_rootSignatureDesc;
        }

        uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
        uint32_t GetNumDescriptors(uint32_t rootIndex) const;

	private:
        friend class std::default_delete<RootSignature>;

        void Destroy();
        void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

        GraphicsDevice* m_device;
        D3D12_ROOT_SIGNATURE_DESC1                  m_rootSignatureDesc;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

        // Need to know the number of descriptors per descriptor table.
        // A maximum of 32 descriptor tables are supported (since a 32-bit
        // mask is used to represent the descriptor tables in the root signature.
        uint32_t m_numDescriptorsPerTable[32];

        // A bit mask that represents the root parameter indices that are
        // descriptor tables for Samplers.
        uint32_t m_samplerTableBitMask;
        // A bit mask that represents the root parameter indices that are
        // CBV, UAV, and SRV descriptor tables.
        uint32_t m_descriptorTableBitMask;
	};

}
