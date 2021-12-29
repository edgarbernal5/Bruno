#pragma once

#include "TrioApiRequisites.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace BrunoEngine
{
	// Vertex struct holding position, normal vector, and texture mapping information.
	struct BRUNO_API_EXPORT VertexPositionNormalTexture
	{
		VertexPositionNormalTexture() = default;

		VertexPositionNormalTexture(const VertexPositionNormalTexture&) = default;
		VertexPositionNormalTexture& operator=(const VertexPositionNormalTexture&) = default;

		VertexPositionNormalTexture(VertexPositionNormalTexture&&) = default;
		VertexPositionNormalTexture& operator=(VertexPositionNormalTexture&&) = default;

		VertexPositionNormalTexture(Vector3 const& position, Vector3 const& normal, Vector2 const& textureCoordinate)
			: m_position(position),
			m_normal(normal),
			m_textureCoordinate(textureCoordinate)
		{ }

#ifdef BRUNO_DIRECTX
		VertexPositionNormalTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR textureCoordinate)
		{
			XMStoreFloat3(&this->m_position, position);
			XMStoreFloat3(&this->m_normal, normal);
			XMStoreFloat2(&this->m_textureCoordinate, textureCoordinate);
		}
#endif

		const Vector3& GetPosition() { return m_position; }
		const Vector3& GetNormal() { return m_normal; }
		const Vector2& GetTexCoord() { return m_textureCoordinate; }

		static VertexDeclaration* GetVertexDeclaration();

		static const int InputElementCount = 3;
#ifdef BRUNO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];

	private:
		Vector3 m_position;
		Vector3 m_normal;
		Vector2 m_textureCoordinate;

		static VertexDeclaration* g_vertexDeclaration;
	};
}
