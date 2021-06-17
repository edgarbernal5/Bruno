#include "stdafx.h"
#include "VertexPositionTexture.h"

#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionTexture::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position .
#ifdef BRUNO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionTexture::InputElementsD3D11[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
		if (g_vertexDeclaration == nullptr)
		{
#ifdef BRUNO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionTexture).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
