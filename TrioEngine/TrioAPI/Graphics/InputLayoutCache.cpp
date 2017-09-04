#include "stdafx.h"
#include "InputLayoutCache.h"

#if TRIO_DIRECTX
#include "VertexDeclaration.h"

namespace Cuado {

	InputLayoutCache::InputLayoutCache(GraphicsDevice* device, std::vector<uint8_t>& byteCode) :
		m_pDevice(device),
		m_vByteCode(byteCode)
	{

	}

	InputLayoutCache::~InputLayoutCache()
	{

	}

	ID3D11InputLayout* InputLayoutCache::Get(VertexDeclaration* vertexDecl)
	{
		auto it = m_cache.find(vertexDecl);
		if (it == m_cache.end())
		{
			ID3D11InputLayout* inputLayout = nullptr;

			const std::vector<D3D11_INPUT_ELEMENT_DESC> inputs = vertexDecl->GetInputElements();

			DX::ThrowIfFailed(
				m_pDevice->GetD3DDevice()->CreateInputLayout(&inputs[0], inputs.size(), &m_vByteCode[0], m_vByteCode.size(), &inputLayout)
			);

			m_cache[vertexDecl] = inputLayout;
			return inputLayout;
		}
		return it->second;
	}
}
#endif