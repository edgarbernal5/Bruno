#pragma once

#include "TrioAPI.h"

#include "IVertexType.h"
#include "SimpleMath.h"

#include "VertexElement.h"
namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	struct TRIOAPI_DLL VertexPositionColor : IVertexType
	{
	public:
		VertexPositionColor(Vector3 position, Color color);

		const Vector3& GetPosition()	{ return m_position; }
		const Color& GetColor()		{ return m_color; }

		static const int InputElementCount = 2;
#if TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
#elif TRIO_OPENGL
		static const VertexElement InputElements[InputElementCount];
#endif
		static VertexDeclaration* GetVertexDeclaration();


	private:
		Vector3 m_position;
		Color m_color;

		static VertexDeclaration* g_VertexDeclaration;
	};
}