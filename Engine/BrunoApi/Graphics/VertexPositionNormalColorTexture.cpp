#include "stdafx.h"
#include "VertexPositionNormalColorTexture.h"

#include "VertexDeclarationHelper.h"

namespace BrunoEngine
{
	VertexDeclaration* VertexPositionNormalColorTexture::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position, normal vector, color, and texture mapping information.
#ifdef BRUNO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionNormalColorTexture::InputElementsD3D11[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionNormalColorTexture::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 0 },
		{ 24, VertexElementFormat::Color, VertexElementUsage::Color, 0 },
		{ 28, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0 }
	};

	VertexDeclaration* VertexPositionNormalColorTexture::GetVertexDeclaration()
	{
		if (g_vertexDeclaration == nullptr)
		{
#ifdef BRUNO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionNormalColorTexture).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
