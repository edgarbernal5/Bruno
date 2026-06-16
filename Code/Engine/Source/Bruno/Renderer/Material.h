#pragma once

#include "Bruno/Content/Asset.h"
#include <string>
#include <map>

#include "Bruno/Platform/DirectX/DescriptorAllocator_Gem.h"
#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"

namespace Bruno::DX
{
	class GraphicsDevice;
}
namespace Bruno
{
	class Texture;
	class AbstractAssetManager;
	
	class Material : public Asset
	{
		BR_RTTI_DECLARATION(Material, Asset);

	public:
		Material();

		AssetType GetAssetType() const override { return AssetType::Material; }

		std::string Name;
		std::map<std::string, AssetHandle> TexturesByName;

		// --- NUEVA INTERFAZ PARA DIRECTX 12 ---

		// 1. Asignar el Pipeline y Root Signature (Compartidos entre muchos materiales)
		void SetPipelineState(std::shared_ptr<DX::GraphicsPipelineState> pso, std::shared_ptr<DX::RootSignature> rootSig);
    
		std::shared_ptr<DX::GraphicsPipelineState> GetPSO() const { return m_pso; }
		std::shared_ptr<DX::RootSignature> GetRootSignature() const { return m_rootSignature; }

		// 2. Construir la tabla de descriptores para este material específico
		void BuildDescriptors(DX::GraphicsDevice* device, DX::DescriptorAllocator* srvAllocator, AbstractAssetManager* assetManager);

		// 3. Obtener el handle para el RenderLoop
		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureDescriptorTable() const;
		
	private:
		// Punteros a los objetos compartidos (No se crean aquí, se inyectan)
		std::shared_ptr<DX::GraphicsPipelineState> m_pso;
		std::shared_ptr<DX::RootSignature> m_rootSignature;

		// La reserva de memoria en el Heap de Descriptores para las texturas de ESTE material
		DX::DescriptorAllocation m_textureDescriptorAllocation;
		bool m_descriptorsBuilt = false;
	};
}
