#pragma once

#include "Bruno/Content/Asset.h"
#include <string>
#include <map>

#include "Bruno/Math/Math.h"
#include "Bruno/Platform/DirectX/DescriptorAllocator.h"

namespace Bruno
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
		//std::map<std::string, AssetHandle> TexturesByName;

		// --- PROPIEDADES PBR (Asset Handles apuntando al AssetManager) ---
		AssetHandle AlbedoMap = 0;       // 0 significa sin textura
		AssetHandle NormalMap = 0;       //
		AssetHandle MetallicRoughnessMap = 0; //

		// --- FACTORES ESCALARES (Por si el modelo no tiene texturas) ---
		Math::Vector4 AlbedoTint = { 1.0f, 1.0f, 1.0f, 1.0f }; //
		float MetallicFactor = 0.0f;                           //
		float RoughnessFactor = 0.5f;
		
		// --- ENLACE CON EL RENDERER (LA LLAVE BINDLESS) ---
		// Este es el ID que el sistema le asignará al instanciarse en la GPU.
		// Lo inicializamos en un valor inválido.
		uint32_t RuntimeMaterialIndex = 0xFFFFFFFF;
		
	private:

		// La reserva de memoria en el Heap de Descriptores para las texturas de ESTE material
		DescriptorAllocation m_textureDescriptorAllocation;
	};
}
