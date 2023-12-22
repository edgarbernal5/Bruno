#pragma once

#include <d3d12.h>
#include "D3DConstants.h"
#include <array>
#include <optional>
#include <wrl/client.h>

namespace Bruno
{
	class ShaderProgram;
	class RootSignature;
	class Resource;
	class Texture;
	class GpuBuffer;
	class PipelineStateObject;

    enum class PipelineType : uint8_t
    {
        Graphics = 0,
        Compute
    };

	struct PipelineInfo
	{
		PipelineStateObject* Pipeline = nullptr;
		std::vector<Texture*> RenderTargets;
		Texture* DepthStencilTarget = nullptr;
	};

	struct PipelineResourceBinding
	{
		uint32_t BindingIndex = 0;
		Resource* Resource{ nullptr };
	};

	class PipelineResourceSpace
	{
	public:
		void SetCBV(GpuBuffer* resource);
		void SetSRV(const PipelineResourceBinding& binding);
		void SetUAV(const PipelineResourceBinding& binding);
		void Lock();

		const GpuBuffer* GetCBV() const { return m_cbv; }
		const std::vector<PipelineResourceBinding>& GetUAVs() const { return m_uavs; }
		const std::vector<PipelineResourceBinding>& GetSRVs() const { return m_srvs; }

		bool IsLocked() const { return m_isLocked; }

	private:
		uint32_t GetIndexOfBindingIndex(const std::vector<PipelineResourceBinding>& bindings, uint32_t bindingIndex);

		//If a resource space needs more than one CBV, it is likely a design flaw, as you want to consolidate these as much
		//as possible if they have the same update frequency (which is contained by a PipelineResourceSpace). Of course,
		//you can freely change this to a vector like the others if you want.
		GpuBuffer* m_cbv = nullptr;
		std::vector<PipelineResourceBinding> m_uavs;
		std::vector<PipelineResourceBinding> m_srvs;
		bool m_isLocked = false;
	};

    struct PipelineResourceMapping
    {
        std::array<std::optional<uint32_t>, Graphics::Core::RESOURCE_SPACES_COUNT> CbvMapping{};
        std::array<std::optional<uint32_t>, Graphics::Core::RESOURCE_SPACES_COUNT> TableMapping{};
    };

    struct PipelineResourceLayout
    {
        std::array<PipelineResourceSpace*, Graphics::Core::RESOURCE_SPACES_COUNT> Spaces{ nullptr };
    };

	struct RenderTargetDesc
	{
		std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> RenderTargetFormats{ DXGI_FORMAT_UNKNOWN };
		uint8_t RenderTargetsCount = 0;
		DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_UNKNOWN;
	};

	struct GraphicsPipelineDesc
	{
		ShaderProgram* VertexShader = nullptr;
		ShaderProgram* PixelShader = nullptr;
		D3D12_RASTERIZER_DESC RasterDesc{};
		D3D12_BLEND_DESC BlendDesc{};
		D3D12_DEPTH_STENCIL_DESC DepthStencilDesc{};
		RenderTargetDesc RenderTargetDesc{};
		DXGI_SAMPLE_DESC SampleDesc{};
		D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	};

	class PipelineStateObject
	{
	public:
		PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		PipelineStateObject(const GraphicsPipelineDesc& desc, std::shared_ptr<RootSignature> rootSignature, PipelineResourceMapping& pipelineResourceMapping);
		virtual ~PipelineStateObject() = default;
		
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_d3d12PipelineState.Get(); }
        RootSignature* GetRootSignature() const { return m_rootSignature.get(); }
	    
        friend class GraphicsContext;
        friend class GraphicsDevice;
    private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12PipelineState;
		std::shared_ptr<RootSignature> m_rootSignature;
		PipelineType m_pipelineType = PipelineType::Graphics;
		PipelineResourceMapping m_pipelineResourceMapping;
	};

	inline GraphicsPipelineDesc GetDefaultGraphicsPipelineDesc()
	{
		GraphicsPipelineDesc desc{};
        desc.RasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.RasterDesc.CullMode = D3D12_CULL_MODE_BACK; //D3D12_CULL_MODE_FRONT
        desc.RasterDesc.FrontCounterClockwise = true;
        desc.RasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.RasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.RasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.RasterDesc.DepthClipEnable = true;
        desc.RasterDesc.MultisampleEnable = false;
        desc.RasterDesc.AntialiasedLineEnable = false;
        desc.RasterDesc.ForcedSampleCount = 0;
        desc.RasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        desc.BlendDesc.AlphaToCoverageEnable = false;
        desc.BlendDesc.IndependentBlendEnable = false;

        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            false, false,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };

        for (uint32_t i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        {
            desc.BlendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
        }

        desc.DepthStencilDesc.DepthEnable = false;
        desc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        desc.DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        desc.DepthStencilDesc.StencilEnable = false;
        desc.DepthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        desc.DepthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
        { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };

        desc.DepthStencilDesc.FrontFace = defaultStencilOp;
        desc.DepthStencilDesc.BackFace = defaultStencilOp;

        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        return desc;
	}
}

