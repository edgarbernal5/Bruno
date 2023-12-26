#pragma once

#include <dxgiformat.h>

namespace Bruno
{
	class PipelineStateObject;

	enum class RenderPsoId : uint8_t
	{
		UnlitColored,
		OpaqueForward,

		Count
	};

	class PipelineStateObjectCache
	{
	public:
		void Initialize(DXGI_FORMAT backbufferFormat, DXGI_FORMAT depthStencilFormat);
		std::shared_ptr<PipelineStateObject> Get(RenderPsoId psoId);

	private:
		std::shared_ptr<Shader> GetShader(RenderPsoId psoId);
		std::unordered_map<RenderPsoId, std::shared_ptr<PipelineStateObject>> m_cache;
	};
}