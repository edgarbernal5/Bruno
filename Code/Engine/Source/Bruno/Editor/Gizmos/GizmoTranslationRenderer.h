#pragma once

#include "Bruno/Platform/DirectX/VertexTypes.h"

namespace Bruno
{
	template<typename TVertex>
	class PrimitiveBatch;

	class GraphicsDevice;
	class GraphicsContext;

	class GizmoTranslationRenderer
	{
	public:
		GizmoTranslationRenderer(GraphicsDevice* device);

		void Render(GraphicsContext* context);

	private:
		std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> m_batch;
	};
}

