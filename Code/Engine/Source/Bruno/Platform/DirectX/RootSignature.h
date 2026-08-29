#pragma once

#include "D3DHelpers.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl/client.h>

#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    enum class VertexFormat : uint8_t;
    enum class DepthMode : uint8_t;
    enum class BlendMode : uint8_t;
    enum class FillMode : uint8_t;
    enum class CullMode : uint8_t;
    enum class ShaderVisibility : uint8_t;
    
    class GraphicsDevice;

    //TODO: En Vulkan, RootSignature se conoce como "PipelineLayout"
    class RootSignature
    {
    public:
        explicit RootSignature(GraphicsDevice& device);
        ~RootSignature() = default;
        
        void AddConstants(uint32_t num32BitValues, uint32_t shaderRegister, uint32_t registerSpace = 0, ShaderVisibility visibility = ShaderVisibility::All);
        void AddConstantBufferView(uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility = ShaderVisibility::All);
        void AddDescriptorTableSRV(uint32_t numDescriptors, uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility = ShaderVisibility::All);
        void AddStaticSampler(uint32_t shaderRegister, uint32_t registerSpace = 0, TextureFilter filter = TextureFilter::Linear, TextureAddressMode addressMode = TextureAddressMode::Wrap, ShaderVisibility visibility = ShaderVisibility::All);
        void Build(RootSignatureFlags flags = RootSignatureFlags::AllowInputAssembler);
        
        size_t ComputeHash(RootSignatureFlags flags) const;
        
        [[nodiscard]] ID3D12RootSignature* GetNative() const { return m_rootSignature.Get(); }
        
    private:
        
        GraphicsDevice& m_device;
        size_t m_hash = 0;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
        
        std::vector<CD3DX12_ROOT_PARAMETER> m_parameters;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_staticSamplers;
        std::vector<std::unique_ptr<CD3DX12_DESCRIPTOR_RANGE[]>> m_descriptorRanges;
    };
}
