#include "stdafx.h"
#include "VertexPositionTexture.h"

#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionTexture::g_pVertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position .
#ifdef TRIO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionTexture::InputElementsD3D11[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionTexture::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0 }
	};

	VertexDeclaration* VertexPositionTexture::GetVertexDeclaration()
	{
		if (g_pVertexDeclaration == nullptr)
		{
#ifdef TRIO_DIRECTX
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionTexture).name(), g_pVertexDeclaration);
		}

		return g_pVertexDeclaration;
	}
}
