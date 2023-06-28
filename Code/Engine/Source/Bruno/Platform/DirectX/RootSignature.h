#pragma once

#include "D3DCommon.h"

namespace Bruno
{
    class GraphicsDevice;

	class RootSignature
	{
	public:
        RootSignature(const CD3DX12_ROOT_SIGNATURE_DESC& rootSignatureDesc);
        virtual ~RootSignature();

        ID3D12RootSignature* GetD3D12RootSignature() const { return m_rootSignature.Get(); }
        const CD3DX12_ROOT_SIGNATURE_DESC& GetRootSignatureDesc() const { return m_rootSignatureDesc; }

	private:
        CD3DX12_ROOT_SIGNATURE_DESC                     m_rootSignatureDesc;
        Microsoft::WRL::ComPtr<ID3D12RootSignature>     m_rootSignature;
	};

}
