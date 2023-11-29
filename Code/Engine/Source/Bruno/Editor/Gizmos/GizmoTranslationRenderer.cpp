#include "brpch.h"
#include "GizmoTranslationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"

namespace Bruno
{
	GizmoTranslationRenderer::GizmoTranslationRenderer(GraphicsDevice* device)
	{
		m_batch = std::make_shared<PrimitiveBatch<VertexPositionNormalColor>>(device, 4096 * 3, 4096);
	}

	void GizmoTranslationRenderer::Render(GraphicsContext* context)
	{
		m_batch->Begin(context);

		m_batch->End();
	}
}