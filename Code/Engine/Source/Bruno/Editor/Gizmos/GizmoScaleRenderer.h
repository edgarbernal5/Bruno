#pragma once

#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"

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

	class GizmoScaleRenderer
	{
	public:
		struct RenderConfig
		{
			float StickRadius = Gizmo::CONE_RADIUS * 0.25f;
			float StickHeight = Gizmo::LINE_LENGTH;
			float ArrowheadRadius = Gizmo::CONE_RADIUS;
			float ArrowheadHeight = Gizmo::CONE_HEIGHT;
			uint32_t Tessellation = Gizmo::TESSELLATION;
			uint32_t StackCount = 3;

			RenderConfig() {}
			RenderConfig(const GizmoConfig& gizmoConfig);
		};
		GizmoScaleRenderer(GraphicsDevice* device, Camera& camera, Surface* surface, std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> batch, RenderConfig renderConfig = RenderConfig());

		void Render(GraphicsContext* context);
		void SetColors(const Math::Color colors[3]);
		void SetWorld(const Math::Matrix& world) { m_gizmoWorld = world; }
		void Update();

	private:
		void CreateBox(const Math::Vector3& size, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world);
		void CreateCylinder(float radius, float height, uint32_t sliceCount, uint32_t stackCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world);

		Camera& m_camera;
		Surface* m_surface;
		std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> m_batch;

		Math::Matrix m_gizmoWorld;
		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_constantBuffers[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];

		std::shared_ptr<Shader>					m_shader;
		std::shared_ptr<RootSignature>			m_rootSignature;
		std::shared_ptr<PipelineStateObject>	m_pipelineObject;
		PipelineResourceSpace					m_meshPerObjectResourceSpace;

		size_t m_xUpperBound;
		size_t m_yUpperBound;

		std::vector<VertexPositionNormalColor>	m_vertices;
		std::vector<uint16_t>					m_indices;
	};
}
