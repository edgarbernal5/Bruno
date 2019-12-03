#pragma once

#include "TrioApiRequisites.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace TrioEngine
{
	// Vertex struct holding position and texture mapping information.
	struct TRIO_API_EXPORT VertexPositionTexture
	{
		VertexPositionTexture() = default;

		VertexPositionTexture(const VertexPositionTexture&) = default;
		VertexPositionTexture& operator=(const VertexPositionTexture&) = default;

		VertexPositionTexture(VertexPositionTexture&&) = default;
		VertexPositionTexture& operator=(VertexPositionTexture&&) = default;

		VertexPositionTexture(Vector3 const& position, Vector2 const& textureCoordinate)
			: m_position(position),
			m_textureCoordinate(textureCoordinate)
		{ }

#ifdef TRIO_DIRECTX
		VertexPositionTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR textureCoordinate)
		{
			XMStoreFloat3(&this->m_position, position);
			XMStoreFloat2(&this->m_textureCoordinate, textureCoordinate);
		}
#endif

		static VertexDeclaration* GetVertexDeclaration();

		static const int InputElementCount = 2;
#ifdef TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];
	private:
		Vector3 m_position;
		Vector2 m_textureCoordinate;

		static VertexDeclaration* g_vertexDeclaration;
	};
}
