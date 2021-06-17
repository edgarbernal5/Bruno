#pragma once

#include "TrioApiRequisites.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace BrunoEngine
{
	class VertexDeclaration;

	// Vertex struct holding position and color information.
	struct BRUNO_API_EXPORT VertexPositionColor
	{
		VertexPositionColor() = default;

		VertexPositionColor(const VertexPositionColor&) = default;
		VertexPositionColor& operator=(const VertexPositionColor&) = default;

		VertexPositionColor(VertexPositionColor&&) = default;
		VertexPositionColor& operator=(VertexPositionColor&&) = default;

		VertexPositionColor(Vector3 const& position, ColorRGBA8 const& color) :
			m_position(position),
			m_color(color)
		{ }

#ifdef BRUNO_DIRECTX
		VertexPositionColor(DirectX::FXMVECTOR position, DirectX::FXMVECTOR color)
		{
			XMStoreFloat3(&this->m_position, position);
			XMStoreColor(&this->m_color, color);
		}
#endif

		const Vector3& GetPosition() { return m_position; }
		const ColorRGBA8& GetColor() { return m_color; }

		static VertexDeclaration* GetVertexDeclaration();

		static const int InputElementCount = 2;
#ifdef BRUNO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];
	private:
		Vector3 m_position;
		ColorRGBA8 m_color;

		static VertexDeclaration* g_vertexDeclaration;
	};
}
