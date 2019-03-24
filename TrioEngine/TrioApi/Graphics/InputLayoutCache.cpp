#include "stdafx.h"
#include "InputLayoutCache.h"

#include "VertexDeclaration.h"
#include "GraphicsDevice.h"

namespace Vago
{
#ifdef TRIO_DIRECTX
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
		auto it = m_mCache.find(vertexDecl);
		if (it == m_mCache.end())
		{
			ID3D11InputLayout* inputLayout = nullptr;

			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputs = vertexDecl->GetD3D11InputElements();

			DX::ThrowIfFailed(
				m_pDevice->GetD3DDevice()->CreateInputLayout(inputs.data(), inputs.size(), m_vByteCode.data(), m_vByteCode.size(), &inputLayout)
			);

			m_mCache[vertexDecl] = inputLayout;
			return inputLayout;
		}
		return it->second;
	}
#endif
}