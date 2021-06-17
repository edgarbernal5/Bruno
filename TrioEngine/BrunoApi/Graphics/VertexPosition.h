#pragma once

#include "TrioApiRequisites.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace BrunoEngine
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

#ifdef BRUNO_DIRECTX
		VertexPosition(DirectX::FXMVECTOR position)
		{
			XMStoreFloat3(&this->m_position, position);
		}
#endif

		const Vector3& GetPosition() { return m_position; }

		static const int InputElementCount = 1;
#ifdef BRUNO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];

		static VertexDeclaration* GetVertexDeclaration();
	private:
		Vector3 m_position;

		static VertexDeclaration* g_vertexDeclaration;
	};
}
