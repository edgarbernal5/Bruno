#include "stdafx.h"
#include "InputLayoutCache.h"

#include "VertexDeclaration.h"
#include "GraphicsDevice.h"

namespace TrioEngine
{
#ifdef BRUNO_DIRECTX
	InputLayoutCache::InputLayoutCache(GraphicsDevice* device, std::vector<uint8_t>& byteCode) :
		m_device(device),
		m_byteCode(byteCode)
	{

	}

	InputLayoutCache::~InputLayoutCache()
	{
		for (std::pair<VertexDeclaration*, ID3D11InputLayout*> inputLayout : m_cache)
		{
			RELEASE_COM(inputLayout.second);
		}

		m_cache.clear();
	}

	ID3D11InputLayout* InputLayoutCache::Get(VertexDeclaration* vertexDecl)
	{
		auto it = m_cache.find(vertexDecl);

		if (it == m_cache.end())
		{
			ID3D11InputLayout* inputLayout = nullptr;

			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputs = vertexDecl->GetD3D11InputElements();

			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateInputLayout(inputs.data(), inputs.size(), m_byteCode.data(), m_byteCode.size(), &inputLayout)
			);

			m_cache[vertexDecl] = inputLayout;
			return inputLayout;
		}
		return it->second;
	}
#endif
}