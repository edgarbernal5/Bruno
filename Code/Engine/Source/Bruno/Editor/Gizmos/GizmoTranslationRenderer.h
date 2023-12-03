#pragma once

#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Platform/DirectX/ConstantBuffer.h"

#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"

namespace Bruno
{
	template<typename TVertex>
	class PrimitiveBatch;

	class GraphicsDevice;
	class GraphicsContext;
	class Camera;
	class Surface;

	class GizmoTranslationRenderer
	{
	public:
		GizmoTranslationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface);

		void Render(GraphicsContext* context);

	private:
		void CreateCone(float radius, float height, uint32_t sliceCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world);
		void CreateCylinder(float radius, float height, uint32_t sliceCount, uint32_t stackCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world);

		Camera& m_camera;
		Surface* m_surface;
		std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> m_batch;

		struct ObjectBuffer
		{
			Math::Matrix World;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_constantBuffers[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];

		std::shared_ptr<Shader>			m_shader;
		std::shared_ptr<RootSignature>	m_rootSignature;
		std::shared_ptr<PipelineStateObject>	m_pipelineObject;
		PipelineResourceSpace			m_meshPerObjectResourceSpace;


		std::vector<VertexPositionNormalColor> m_vertices;
		std::vector<uint16_t> m_indices;
	};
}

