#pragma once

#include "TrioApiDecl.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace TrioEngine
{
	// Vertex struct holding position information.
	struct VertexPosition
	{
		VertexPosition() = default;

		VertexPosition(const VertexPosition&) = default;
		VertexPosition& operator=(const VertexPosition&) = default;

		VertexPosition(VertexPosition&&) = default;
		VertexPosition& operator=(VertexPosition&&) = default;

		VertexPosition(Vector3 const& position)
			: m_position(position)
		{ }

#ifdef TRIO_DIRECTX
		VertexPosition(DirectX::FXMVECTOR position)
		{
			XMStoreFloat3(&this->m_position, position);
		}
#endif

		const Vector3& GetPosition() { return m_position; }

		static const int InputElementCount = 1;
#ifdef TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];

		static VertexDeclaration* GetVertexDeclaration();
	private:
		Vector3 m_position;

		static VertexDeclaration* g_pVertexDeclaration;
	};
}
