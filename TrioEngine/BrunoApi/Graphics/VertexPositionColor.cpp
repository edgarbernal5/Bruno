#include "stdafx.h"
#include "VertexPositionColor.h"

#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionColor::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position and color information.
#ifdef BRUNO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionColor::InputElementsD3D11[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionColor::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Color, VertexElementUsage::Color, 0 }
	};
	static_assert(sizeof(VertexPositionColor) == 16, "Vertex struct/layout mismatch");

	VertexDeclaration* VertexPositionColor::GetVertexDeclaration()
	{
		if (g_vertexDeclaration == nullptr)
		{
#ifdef BRUNO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionColor).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
