#pragma once

#include <Bruno.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/RootSignature.h>
#include <Bruno/Platform/DirectX/PipelineStateObject.h>
#include <Bruno/Platform/DirectX/ConstantBuffer.h>
#include <Bruno/Platform/DirectX/Texture.h>

namespace Bruno
{
	struct RenderItem;

	class PlayerGame : public Game
	{
	public:
		PlayerGame(const GameParameters& parameters);

	protected:
		void DoOnInitialize() override;
		void OnClientSizeChanged() override;
		void DoOnUpdate(const GameTimer& timer) override;
		void DoOnDraw() override;

	private:
		std::unique_ptr<Surface> m_surface;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<Texture> m_texture;
		//std::unique_ptr<RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::unique_ptr<PipelineStateObject> m_pipelineState;

		std::vector<std::shared_ptr<RenderItem>> m_renderItems;

		struct ObjectBuffer
		{
			DirectX::XMFLOAT4X4 m_world;
		};
		std::unique_ptr<ConstantBuffer<ObjectBuffer>> m_objectBuffer;
	};

}