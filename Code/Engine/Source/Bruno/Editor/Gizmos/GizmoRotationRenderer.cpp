#include "brpch.h"
#include "GizmoRotationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

namespace Bruno
{
	GizmoRotationRenderer::GizmoRotationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface) :
		m_camera(camera),
		m_surface(surface)
	{
		m_batch = std::make_shared<PrimitiveBatch<VertexPositionNormalColor>>(device, 4096 * 3, 4096);

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_constantBuffers[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}

		m_shader = std::make_shared<Shader>(L"Shaders/UnlitColor.hlsl");

		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[0].get());
		m_meshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &m_meshPerObjectResourceSpace;

		PipelineResourceMapping resourceMapping;
		m_rootSignature = std::make_unique<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_shader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_shader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surface->GetDepthBufferFormat();
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineObject = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);


	}

	void GizmoRotationRenderer::Render(GraphicsContext* context)
	{
		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_pipelineObject.get();
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;

		context->SetPipeline(pipeline);
		context->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

		m_batch->Begin(context);
		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_indices.data(), m_indices.size(), m_vertices.data(), m_vertices.size());
		m_batch->End();
	}

	void GizmoRotationRenderer::SetColors(const Math::Color colors[3])
	{

	}

	void GizmoRotationRenderer::Update()
	{
		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_gizmoWorld * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_constantBuffers[frameIndex]->SetMappedData(objectBuffer);

		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[frameIndex].get());
	}
}