#pragma once

#include "D3DCommon.h"
#include <array>

namespace Bruno
{
	class ShaderProgram;
	class RootSignature;

	struct RenderTargetDesc
	{
		std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> mRenderTargetFormats{ DXGI_FORMAT_UNKNOWN };
		uint8_t mNumRenderTargets = 0;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;
	};

	struct GraphicsPipelineDesc
	{
		ShaderProgram* mVertexShader = nullptr;
		ShaderProgram* mPixelShader = nullptr;
		D3D12_RASTERIZER_DESC mRasterDesc{};
		D3D12_BLEND_DESC mBlendDesc{};
		D3D12_DEPTH_STENCIL_DESC mDepthStencilDesc{};
		RenderTargetDesc mRenderTargetDesc{};
		DXGI_SAMPLE_DESC mSampleDesc{};
		D3D12_PRIMITIVE_TOPOLOGY_TYPE mTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	};

	class PipelineStateObject
	{
	public:
		PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		PipelineStateObject(const GraphicsPipelineDesc& desc, RootSignature* rootSignature);
		virtual ~PipelineStateObject() = default;
		
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_d3d12PipelineState.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12PipelineState;
	};

	inline GraphicsPipelineDesc GetDefaultGraphicsPipelineDesc()
	{
        GraphicsPipelineDesc desc;
        desc.mRasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.mRasterDesc.CullMode = D3D12_CULL_MODE_BACK;
        desc.mRasterDesc.FrontCounterClockwise = false;
        desc.mRasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.mRasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.mRasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.mRasterDesc.DepthClipEnable = true;
        desc.mRasterDesc.MultisampleEnable = false;
        desc.mRasterDesc.AntialiasedLineEnable = false;
        desc.mRasterDesc.ForcedSampleCount = 0;
        desc.mRasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        desc.mBlendDesc.AlphaToCoverageEnable = false;
        desc.mBlendDesc.IndependentBlendEnable = false;

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
            desc.mBlendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
        }

        desc.mDepthStencilDesc.DepthEnable = false;
        desc.mDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        desc.mDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        desc.mDepthStencilDesc.StencilEnable = false;
        desc.mDepthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        desc.mDepthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
        { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };

        desc.mDepthStencilDesc.FrontFace = defaultStencilOp;
        desc.mDepthStencilDesc.BackFace = defaultStencilOp;

        desc.mSampleDesc.Count = 1;
        desc.mSampleDesc.Quality = 0;

        return desc;
	}
}

