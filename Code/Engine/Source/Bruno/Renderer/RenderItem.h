#pragma once

#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"
#include "Bruno/Renderer/Model.h"

namespace Bruno
{
	struct RenderItem
	{
		RenderItem() = default;

		uint32_t IndexCount{ 0 };
		uint32_t StartIndexLocation{ 0 };
		int BaseVertexLocation{ 0 };

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

		ModelMaterial* Material{ nullptr };
		std::shared_ptr<IndexBuffer> IndexBuffer;
		std::shared_ptr<VertexBuffer> VertexBuffer;
	};
}