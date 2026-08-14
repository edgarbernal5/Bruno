#pragma once

#include "D3DHelpers.h"
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl/client.h>

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
        
        // Métodos agnósticos para construir la firma
        void AddConstantBufferView(uint32_t shaderRegister, uint32_t registerSpace, ShaderVisibility visibility);
        void AddDescriptorTableSRV(uint32_t numDescriptors, uint32_t shaderRegister, ShaderVisibility visibility);
        void AddStaticSampler(uint32_t shaderRegister, ShaderVisibility visibility);
        
        void Build();
        
        void Initialize(
            UINT numParameters, 
            const CD3DX12_ROOT_PARAMETER* parameters,
            UINT numSamplers = 0,
            const CD3DX12_STATIC_SAMPLER_DESC* samplers = nullptr,
            D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );
        
        [[nodiscard]] ID3D12RootSignature* GetNative() const { return m_rootSignature.Get(); }
        
    private:
        D3D12_CULL_MODE GetDX12CullMode(CullMode mode);
        
        GraphicsDevice& m_device;
        
        std::vector<CD3DX12_ROOT_PARAMETER> m_parameters;
        std::vector<CD3DX12_DESCRIPTOR_RANGE> m_descriptorRanges;
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_samplers;
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    };
}
