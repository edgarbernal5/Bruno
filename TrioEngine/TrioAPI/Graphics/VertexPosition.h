#pragma once

#include "TrioAPI.h"

#include "IVertexType.h"
#include "SimpleMath.h"

namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	class TRIOAPI_DLL VertexPosition : public IVertexType
	{
	public:
		VertexPosition(DirectX::SimpleMath::Vector3 position);

		DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }

		static const int InputElementCount = 1;
#if TRIO_DIRECTX
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
#elif TRIO_OPENGL
		static const VertexElement InputElements[InputElementCount];
#endif
		static VertexDeclaration* GetVertexDeclaration();

	private:
		DirectX::SimpleMath::Vector3 m_position;

		static VertexDeclaration* g_VertexDeclaration;
	};
}