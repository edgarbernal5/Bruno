#include "brpch.h"
#include "SceneRenderer.h"

#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/RenderItem.h"

#include "Bruno/Platform/DirectX/Texture.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include "Bruno/Platform/DirectX/RootSignature.h"
#include "Bruno/Platform/DirectX/Shader.h"
#include "Bruno/Platform/DirectX/Surface.h"

namespace Bruno
{
	SceneRenderer::SceneRenderer(std::shared_ptr<Scene> scene, Surface* surface) :
		m_scene(scene),
		m_surface(surface)
	{
		m_opaqueShader = std::make_unique<Shader>(L"Shaders/Opaque.hlsl");

		PipelineResourceBinding textureBinding;
		textureBinding.BindingIndex = 0;

		m_meshPerObjectResourceSpace.SetCBV(scene->m_objectBuffer[0].get());
		m_meshPerObjectResourceSpace.SetSRV(textureBinding);
		m_meshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &m_meshPerObjectResourceSpace;

		PipelineResourceMapping resourceMapping;
		m_rootSignature = std::make_unique<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surface->GetDepthBufferFormat();
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineState = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);
	}

	void SceneRenderer::OnRender(GraphicsContext* graphicsContext)
	{
		auto device = Graphics::GetDevice();

		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		for (auto& item : m_scene->GetRenderItems()) {
			auto texture = item->Material->TexturesByName["Texture"];
			if (texture != nullptr && texture->IsReady()) {
				if (!item->IndexBuffer->IsReady() || !item->VertexBuffer->IsReady())
					continue;

				graphicsContext->SetVertexBuffer(*item->VertexBuffer);
				graphicsContext->SetIndexBuffer(*item->IndexBuffer);

				PipelineResourceBinding textureBinding;
				textureBinding.BindingIndex = 0;
				textureBinding.Resource = texture.get();

				m_meshPerObjectResourceSpace.SetCBV(m_scene->m_objectBuffer[device->GetFrameId()].get());
				m_meshPerObjectResourceSpace.SetSRV(textureBinding);

				PipelineInfo pipeline;
				pipeline.Pipeline = m_pipelineState.get();
				pipeline.RenderTargets.push_back(&backBuffer);
				pipeline.DepthStencilTarget = &depthBuffer;

				graphicsContext->SetPipeline(pipeline);
				graphicsContext->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

				graphicsContext->SetPrimitiveTopology(item->PrimitiveType);
				graphicsContext->DrawIndexedInstanced(item->IndexCount,
					1,
					item->StartIndexLocation,
					item->BaseVertexLocation,
					0);
			}
		}
	}
}