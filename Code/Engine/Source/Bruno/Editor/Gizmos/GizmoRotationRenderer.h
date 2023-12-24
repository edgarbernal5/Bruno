#pragma once

#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Platform/DirectX/RenderObjectBinding.h"

#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"

#include "Bruno/Editor/Gizmos/GizmoConfig.h"

namespace Bruno
{
	template<typename TVertex>
	class PrimitiveBatch;

	class GraphicsDevice;
	class GraphicsContext;
	class Camera;
	class Surface;
	struct GizmoObjectBuffer;

	class GizmoRotationRenderer
	{
	public:
		struct RenderConfig
		{
			float Radius = Gizmo::GIZMO_LENGTH;
			uint32_t RingTessellation = Gizmo::RING_TESSELLATION;
			float Thickness = Gizmo::CONE_RADIUS;

			RenderConfig() {}
			RenderConfig(const GizmoConfig& gizmoConfig);
		};
		GizmoRotationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface, std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> batch, RenderConfig renderConfig = RenderConfig());

		void Render(GraphicsContext* context);
		void SetColors(const Math::Color colors[3]);
		void SetWorld(const Math::Matrix& world) { m_gizmoWorld = world; }
		void Update();

	private:
		void CreateHalfTorus(float radius, float thickness, size_t tessellation, float angleStart, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world);

		Camera& m_camera;
		Surface* m_surface;
		std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> m_batch;

		Math::Matrix m_gizmoWorld;
		RenderObjectBinding<GizmoObjectBuffer> m_renderObjectBindingDesc;

		RenderConfig m_renderConfig;
		size_t m_xUpperBound;
		size_t m_yUpperBound;
		Math::Color m_axisColors[3]{ Math::Color(1.0f,0.0f,0.0f,1.0f),Math::Color(0.0f,1.0f,0.0f,1.0f), Math::Color(0.0f,0.0f,1.0f,1.0f) };

		std::vector<VertexPositionNormalColor>	m_vertices;
		std::vector<uint16_t>					m_indices;
	};
}
