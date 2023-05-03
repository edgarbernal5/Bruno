#pragma once

#include <Bruno.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>
//#include <Bruno/Platform/DirectX/RootSignature.h>

namespace Bruno
{
	class PlayerGame : public Game
	{
	public:
		PlayerGame(const GameParameters& parameters);

	protected:
		void DoOnInitialize() override;
		void Tick() override;
		void OnClientSizeChanged() override;

	private:
		std::unique_ptr<Surface> m_surface;
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
	};

}