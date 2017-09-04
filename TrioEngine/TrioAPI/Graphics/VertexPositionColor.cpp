#include "stdafx.h"
#include "VertexPositionColor.h"

#include "VertexDeclaration.h"
#include "VertexDeclarationHelper.h"

namespace Cuado
{
	VertexDeclaration* VertexPositionColor::g_VertexDeclaration = nullptr;

#if TRIO_DIRECTX
	const D3D11_INPUT_ELEMENT_DESC VertexPositionColor::InputElements[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#elif TRIO_OPENGL
	const VertexElement VertexPositionColor::InputElements[] =
	{
		{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 }
	};
#endif

	VertexPositionColor::VertexPositionColor(Vector3 position, Color color) :
		m_position(position),
		m_color(color)
	{

	}

	VertexDeclaration* VertexPositionColor::GetVertexDeclaration()
	{
		if (g_VertexDeclaration == nullptr)
		{
			g_VertexDeclaration = VertexDeclarationHelper::GetVertexDeclaration(InputElementCount, InputElements);
			VertexDeclarationHelper::AddVertexDeclaration(typeid(VertexPositionColor).name(), g_VertexDeclaration);
		}

		return g_VertexDeclaration;
	}
}