#pragma once

#include "Bruno/Platform/DirectX/ConstantBuffer.h"
#include "Bruno/Platform/DirectX/PipelineStateObject.h"
#include "Bruno/Platform/DirectX/Shader.h"

namespace Bruno
{
	template<typename TCBuffer>
	struct RenderObjectBinding
	{
		Shader* Shader;
		PipelineStateObject* Pipeline;
		PipelineResourceSpace Space;
		std::shared_ptr<ConstantBuffer<TCBuffer>> CBuffers[Graphics::Core::FRAMES_IN_FLIGHT_COUNT];
	};
}