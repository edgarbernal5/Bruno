#include "stdafx.h"
#include "VertexPositionNormalTexture.h"

#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionNormalTexture::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position .
#ifdef TRIO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalTexture::InputElementsD3D11[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionNormalTexture::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 0 },
		{ 24, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0 }
	};

	VertexDeclaration* VertexPositionNormalTexture::GetVertexDeclaration()
	{
		if (g_vertexDeclaration == nullptr)
		{
#ifdef TRIO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionNormalTexture).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
