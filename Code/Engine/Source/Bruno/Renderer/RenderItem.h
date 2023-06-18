#pragma once

#include <Bruno/Platform/DirectX/D3DCommon.h>
#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>

namespace Bruno
{
	struct RenderItem
	{
		RenderItem() = default;

		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		int BaseVertexLocation = 0;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::shared_ptr<IndexBuffer> IndexBuffer;
		std::shared_ptr<VertexBuffer> VertexBuffer;
	};
}