#include "brpch.h"
#include "RootSignature.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    RootSignature::RootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
        : m_rootSignatureDesc{}
        , m_numDescriptorsPerTable{ 0 }
        , m_samplerTableBitMask(0)
        , m_descriptorTableBitMask(0)
    {
        SetRootSignatureDesc(rootSignatureDesc);
    }

    RootSignature::~RootSignature()
    {
        Destroy();
    }

    void RootSignature::Destroy()
    {
        for (uint32_t i = 0; i < m_rootSignatureDesc.NumParameters; ++i)
        {
            const D3D12_ROOT_PARAMETER1& rootParameter = m_rootSignatureDesc.pParameters[i];
            if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            {
                delete[] rootParameter.DescriptorTable.pDescriptorRanges;
            }
        }

        delete[] m_rootSignatureDesc.pParameters;
        m_rootSignatureDesc.pParameters = nullptr;
        m_rootSignatureDesc.NumParameters = 0;

        delete[] m_rootSignatureDesc.pStaticSamplers;
        m_rootSignatureDesc.pStaticSamplers = nullptr;
        m_rootSignatureDesc.NumStaticSamplers = 0;

        m_descriptorTableBitMask = 0;
        m_samplerTableBitMask = 0;

        memset(m_numDescriptorsPerTable, 0, sizeof(m_numDescriptorsPerTable));
    }

    void RootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
    {
        // Make sure any previously allocated root signature description is cleaned
        // up first.
        Destroy();

        uint32_t             numParameters = rootSignatureDesc.NumParameters;
        D3D12_ROOT_PARAMETER1* pParameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

        for (uint32_t i = 0; i < numParameters; ++i)
        {
            const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc.pParameters[i];
            pParameters[i] = rootParameter;

            if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            {
                uint32_t                     numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
                D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges =
                    numDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges] : nullptr;

                memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges,
                    sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

                pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
                pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

                // Set the bit mask depending on the type of descriptor table.
                if (numDescriptorRanges > 0)
                {
                    switch (pDescriptorRanges[0].RangeType)
                    {
                    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        m_descriptorTableBitMask |= (1 << i);
                        break;
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                        m_samplerTableBitMask |= (1 << i);
                        break;
                    }
                }

                // Count the number of descriptors in the descriptor table.
                for (uint32_t j = 0; j < numDescriptorRanges; ++j)
                {
                    m_numDescriptorsPerTable[i] += pDescriptorRanges[j].NumDescriptors;
                }
            }
        }

        m_rootSignatureDesc.NumParameters = numParameters;
        m_rootSignatureDesc.pParameters = pParameters;

        uint32_t                       numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
        D3D12_STATIC_SAMPLER_DESC* pStaticSamplers =
            numStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers] : nullptr;

        if (pStaticSamplers)
        {
            memcpy(pStaticSamplers, rootSignatureDesc.pStaticSamplers,
                sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
        }

        m_rootSignatureDesc.NumStaticSamplers = numStaticSamplers;
        m_rootSignatureDesc.pStaticSamplers = pStaticSamplers;

        D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
        m_rootSignatureDesc.Flags = flags;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc;
        versionRootSignatureDesc.Init_1_1(numParameters, pParameters, numStaticSamplers, pStaticSamplers, flags);

        GraphicsDevice* device = Graphics::GetDevice();
        D3D_ROOT_SIGNATURE_VERSION highestVersion = device->GetHighestRootSignatureVersion();

        // Serialize the root signature.
        Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        auto d3d12Device = device->GetD3DDevice();
        try
        {
            ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc, highestVersion,
                &rootSignatureBlob, &errorBlob));

            // Create the root signature.
            ThrowIfFailed(d3d12Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
                rootSignatureBlob->GetBufferSize(),
                IID_PPV_ARGS(&m_rootSignature)));
        }
        catch (std::exception exc)
        {
            const char* errStr = (const char*)errorBlob->GetBufferPointer();
            BR_CORE_ERROR << "Error creating root signature. " << errStr;
        }
    }

    uint32_t RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const
    {
        uint32_t descriptorTableBitMask = 0;
        switch (descriptorHeapType)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            descriptorTableBitMask = m_descriptorTableBitMask;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            descriptorTableBitMask = m_samplerTableBitMask;
            break;
        }

        return descriptorTableBitMask;
    }

    uint32_t RootSignature::GetNumDescriptors(uint32_t rootIndex) const
    {
        assert(rootIndex < 32);
        return m_numDescriptorsPerTable[rootIndex];
    }

}