#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/timer.hpp>

#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>

namespace Bruno
{
	class Surface;

	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window);
		~ScenePanel() = default;

	private:
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		//Scene* m_scene;

		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		//std::unique_ptr<RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

		// Depth buffer.
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
		// Descriptor heap for depth buffer.
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

		nana::timer m_timer;
	};
}
