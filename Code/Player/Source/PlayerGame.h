#pragma once

#include <Bruno.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/PipelineStateObject.h>
#include <Bruno/Platform/DirectX/ConstantBuffer.h>

namespace Bruno
{
	class PlayerGame : public Game
	{
	public:
		PlayerGame(const GameParameters& parameters);

	protected:
		void DoOnInitialize() override;
		void OnTick() override;
		void OnClientSizeChanged() override;

	private:
		std::unique_ptr<Surface> m_surface;
		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		//std::unique_ptr<RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;

		struct ObjectBuffer
		{
			DirectX::XMMATRIX m_world;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer;
	};

}