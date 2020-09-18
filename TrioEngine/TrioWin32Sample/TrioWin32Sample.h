#pragma once

#include "PlatformDirectives.h"
#include "Game.h"

#include "Graphics/Effect.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"

#include "Graphics/VertexDeclaration.h"

#include "Graphics/VertexPositionColorTexture.h"
#include "Graphics/VertexPositionColor.h"

#include "Graphics/RasterizerState.h"
#include "Graphics/SamplerState.h"

#include "Math/MathVector.h"

using namespace TrioEngine;

namespace TrioWin32
{
	class DemoGame : public Game
	{
	public:
		DemoGame();

	protected:
		void Draw(StepTimer const& timer) override;
		void Update(StepTimer const& timer) override;
		void Initialize() override;

		IndexBuffer* m_pIndexBuffer, *m_gridIdxBuffer;
		VertexBuffer* m_pVertexBuffer, *m_gridVertexBuffer;

		Effect* m_pEffect, *m_gridEffect;

		void Magic();

		float mRadius;
		Vector3 m_position;


	};
}
