#include "stdafx.h"
#include "VertexPositionColorTexture.h"

//#include "VertexDeclaration.h"
#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionColorTexture::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position, color, and texture mapping information.
#ifdef BRUNO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionColorTexture::InputElementsD3D11[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionColorTexture::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Color, VertexElementUsage::Color, 0 },
		{ 16, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0 }
	};

	VertexDeclaration* VertexPositionColorTexture::GetVertexDeclaration()
	{
		if (g_vertexDeclaration == nullptr)
		{
#ifdef BRUNO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionColorTexture).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
