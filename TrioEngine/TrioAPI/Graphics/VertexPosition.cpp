#include "stdafx.h"
#include "VertexPosition.h"

#include "VertexDeclaration.h"
#include "VertexDeclarationHelper.h"

namespace Cuado
{
	VertexDeclaration* VertexPosition::g_VertexDeclaration = nullptr;

#if TRIO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPosition::InputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static_assert(sizeof(VertexPosition) == 12, "Vertex struct/layout mismatch");
#elif TRIO_OPENGL
	const VertexElement VertexPosition::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 }
	};
#endif

	VertexPosition::VertexPosition(DirectX::SimpleMath::Vector3 const& position) :
		m_position(position)
	{

	}

	VertexDeclaration* VertexPosition::GetVertexDeclaration()
	{
		if (g_VertexDeclaration == nullptr)
		{
			g_VertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPosition).name(), g_VertexDeclaration);
		}

		return g_VertexDeclaration;
	}
}