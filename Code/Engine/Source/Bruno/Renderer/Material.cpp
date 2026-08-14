#include "brpch.h"
#include "Material.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/Texture2D.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Material);

	Material::Material() : m_textureDescriptorAllocation()
	{
		m_handle = {};
	}

	void Material::SetPipelineState(std::shared_ptr<GraphicsPipelineState> pso, std::shared_ptr<RootSignature> rootSig)
	{
		m_pso = pso;
		m_rootSignature = rootSig;
	}

	void Material::BuildDescriptors(GraphicsDevice* device, DescriptorAllocator* srvAllocator, AbstractAssetManager* assetManager)
	{
		if (m_descriptorsBuilt || TexturesByName.empty())
		{
			return;
		}

		// 1. Pedir espacio contiguo en el Heap para la cantidad de texturas que tenemos
		uint32_t textureCount = static_cast<uint32_t>(TexturesByName.size());
		m_textureDescriptorAllocation = srvAllocator->Allocate(textureCount);

		// 2. Crear un Shader Resource View (SRV) por cada textura
		uint32_t offset = 0;
		for (const auto& [textureName, assetHandle] : TexturesByName)
		{
			// Resolvemos el handle usando el AssetManager para obtener el Texture2D real
			std::shared_ptr<Texture2D> texture = assetManager->GetAsset<Texture2D>(assetHandle);
			if (texture)
			{
				// 1. Obtener el destino (Dónde vamos a escribir en nuestro nuevo heap contiguo)
				D3D12_CPU_DESCRIPTOR_HANDLE destHandle = m_textureDescriptorAllocation.GetCPUHandle(offset);
            
				// 2. Obtener el origen (El SRV original que tu textura ya creó en su constructor)
				D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = texture->GetSRV();
            
				// 3. Copiar (1 descriptor, de origen a destino, tipo CBV_SRV_UAV)
				device->GetNativeDevice()->CopyDescriptorsSimple(
					1, 
					destHandle, 
					srcHandle, 
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
				);
			}
			offset++;
		}

		m_descriptorsBuilt = true;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE Material::GetTextureDescriptorTable() const
	{
		// Retornamos el inicio del bloque contiguo en la memoria de la GPU
		return m_textureDescriptorAllocation.GPU;
	}
}
