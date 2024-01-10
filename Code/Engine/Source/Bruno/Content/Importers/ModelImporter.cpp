#include "brpch.h"
#include "ModelImporter.h"

namespace Bruno
{
	std::map<ModelImporter::TextureType, std::pair<uint32_t, std::string>> ModelImporter::g_textureTypeMappings;

	ModelImporter::ModelImporter()
	{
		if (g_textureTypeMappings.size() > 0)
			return;

		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDiffuse] = std::make_pair(aiTextureType_DIFFUSE, "Texture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularMap] = std::make_pair(aiTextureType_SPECULAR, "SpecularTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeAmbient] = std::make_pair(aiTextureType_AMBIENT, "AmbientTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeEmissive] = std::make_pair(aiTextureType_EMISSIVE, "EmissiveTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeHeightmap] = std::make_pair(aiTextureType_HEIGHT, "HeightTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeNormalMap] = std::make_pair(aiTextureType_NORMALS, "NormalTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularPowerMap] = std::make_pair(aiTextureType_SHININESS, "SpecularPowerTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDisplacementMap] = std::make_pair(aiTextureType_DISPLACEMENT, "DisplacementTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeLightMap] = std::make_pair(aiTextureType_LIGHTMAP, "LightMapTexture");
	}

	bool ModelImporter::TryImport(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset)
	{

		return false;
	}
}