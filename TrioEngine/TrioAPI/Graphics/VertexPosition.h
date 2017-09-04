#pragma once

#include "TrioAPI.h"

#include "IVertexType.h"
#include "SimpleMath.h"


#if TRIO_OPENGL
#include "VertexElement.h"
#endif
namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	struct TRIOAPI_DLL VertexPosition : IVertexType
	{
	public:
		VertexPosition() = default;

		VertexPosition(Vector3 const& position);

		const Vector3& GetPosition() { return m_position; }

		static const int InputElementCount = 1;
#if TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
#elif TRIO_OPENGL
		static const VertexElement InputElements[InputElementCount];
#endif
		static VertexDeclaration* GetVertexDeclaration();


	private:
		Vector3 m_position;

		static VertexDeclaration* g_VertexDeclaration;
	};
}