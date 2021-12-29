#pragma once

#include "TrioApiRequisites.h"

#include "../Math/MathVector.h"
#include "VertexElement.h"

namespace BrunoEngine
{
	// Vertex struct holding position, normal vector, color, and texture mapping information.
	struct BRUNO_API_EXPORT VertexPositionNormalColorTexture
	{
		VertexPositionNormalColorTexture() = default;

		VertexPositionNormalColorTexture(const VertexPositionNormalColorTexture&) = default;
		VertexPositionNormalColorTexture& operator=(const VertexPositionNormalColorTexture&) = default;

		VertexPositionNormalColorTexture(VertexPositionNormalColorTexture&&) = default;
		VertexPositionNormalColorTexture& operator=(VertexPositionNormalColorTexture&&) = default;

		VertexPositionNormalColorTexture(Vector3 const& position, Vector3 const& normal, ColorRGBA8 const& color, Vector2 const& textureCoordinate)
			: m_position(position),
			m_normal(normal),
			m_color(color),
			m_textureCoordinate(textureCoordinate)
		{ }

#ifdef BRUNO_DIRECTX
		VertexPositionNormalColorTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR color, DirectX::CXMVECTOR textureCoordinate)
		{
			XMStoreFloat3(&this->m_position, position);
			XMStoreFloat3(&this->m_normal, normal);
			XMStoreColor(&this->m_color, color);
			XMStoreFloat2(&this->m_textureCoordinate, textureCoordinate);
		}
#endif
		const Vector3& GetPosition() { return m_position; }
		const Vector3& GetNormal() { return m_normal; }
		const ColorRGBA8& GetColor() { return m_color; }
		const Vector2& GetTexCoord() { return m_textureCoordinate; }

		static VertexDeclaration* GetVertexDeclaration();

		static const int InputElementCount = 4;
#ifdef BRUNO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElementsD3D11[InputElementCount];
#endif
		static const VertexElement InputElements[InputElementCount];

	private:
		Vector3 m_position;
		Vector3 m_normal;
		ColorRGBA8 m_color;
		Vector2 m_textureCoordinate;

		static VertexDeclaration* g_vertexDeclaration;
	};
}
