#include "stdafx.h"
#include "VertexPositionColor.h"

#include "VertexDeclarationHelper.h"

namespace TrioEngine
{
	VertexDeclaration* VertexPositionColor::g_pVertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position and color information.
#ifdef TRIO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionColor::InputElementsD3D11[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPositionColor::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
		{ 12, VertexElementFormat::Vector4, VertexElementUsage::Color, 0 }
	};
	static_assert(sizeof(VertexPositionColor) == 28, "Vertex struct/layout mismatch");

	VertexDeclaration* VertexPositionColor::GetVertexDeclaration()
	{
		if (g_pVertexDeclaration == nullptr)
		{
#ifdef TRIO_DIRECTX
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_pVertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionColor).name(), g_pVertexDeclaration);
		}

		return g_pVertexDeclaration;
	}
}
