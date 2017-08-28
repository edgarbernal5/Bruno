#pragma once

#include "TrioAPI.h"

#include "IVertexType.h"
#include "SimpleMath.h"

namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	struct TRIOAPI_DLL VertexPositionColor : IVertexType
	{
	public:
		VertexPositionColor(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Color color);

		const DirectX::SimpleMath::Vector3& GetPosition()	{ return m_position; }
		const DirectX::SimpleMath::Color& GetColor()		{ return m_color; }

		static const int InputElementCount = 2;
#if TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
#elif TRIO_OPENGL
		static const VertexElement InputElements[InputElementCount];
#endif
		static VertexDeclaration* GetVertexDeclaration();


	private:
		DirectX::SimpleMath::Vector3 m_position;
		DirectX::SimpleMath::Color m_color;

		static VertexDeclaration* g_VertexDeclaration;
	};
}