#include "stdafx.h"
#include "VertexPosition.h"

#include "VertexDeclarationHelper.h"

namespace BrunoEngine
{
	VertexDeclaration* VertexPosition::g_vertexDeclaration = nullptr;

	//--------------------------------------------------------------------------------------
	// Vertex struct holding position information.
#ifdef BRUNO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPosition::InputElementsD3D11[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#endif
	const VertexElement VertexPosition::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 }
	};

	VertexDeclaration* VertexPosition::GetVertexDeclaration()
	{
		if (g_vertexDeclaration == nullptr)
		{
#ifdef BRUNO_DIRECTX
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElementsD3D11, InputElements);
#else
			g_vertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
#endif
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPosition).name(), g_vertexDeclaration);
		}

		return g_vertexDeclaration;
	}
}
