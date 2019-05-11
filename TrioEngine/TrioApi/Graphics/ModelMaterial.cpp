#include "stdafx.h"
#include "ModelMaterial.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Utils/StringUtility.h"

namespace TrioEngine
{
	std::map<ModelMaterial::TextureType, uint32_t> ModelMaterial::g_mTextureTypeMappings;

	ModelMaterial::ModelMaterial()
	{
		InitializeTextureTypeMappings();
	}


	ModelMaterial::ModelMaterial(aiMaterial* material)
	{
		InitializeTextureTypeMappings();

		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		m_csName = name.C_Str();

		for (TextureType textureType = (TextureType)0; textureType < ModelMaterial::TextureType::TextureTypeEnd; textureType = (TextureType)(textureType + 1))
		{
			aiTextureType mappedTextureType = (aiTextureType)g_mTextureTypeMappings[textureType];

			UINT textureCount = material->GetTextureCount(mappedTextureType);
			if (textureCount > 0)
			{
				std::vector<std::wstring>* textures = new std::vector<std::wstring>();
				m_mTextures.insert(std::pair<TextureType, std::vector<std::wstring>*>(textureType, textures));

				textures->reserve(textureCount);
				for (UINT textureIndex = 0; textureIndex < textureCount; textureIndex++)
				{
					aiString path;
					if (material->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS)
					{
						std::wstring wPath;
						TrioUtils::StringUtility::ToWideString(path.C_Str(), wPath);
						
						textures->push_back(wPath);
					}
				}
			}
		}
	}

	ModelMaterial::~ModelMaterial()
	{

	}

	void ModelMaterial::InitializeTextureTypeMappings()
	{
		if (g_mTextureTypeMappings.size() != ModelMaterial::TextureType::TextureTypeEnd)
		{
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeDifffuse] = aiTextureType_DIFFUSE;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeSpecularMap] = aiTextureType_SPECULAR;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeAmbient] = aiTextureType_AMBIENT;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeHeightmap] = aiTextureType_HEIGHT;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeNormalMap] = aiTextureType_NORMALS;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
			g_mTextureTypeMappings[ModelMaterial::TextureType::TextureTypeLightMap] = aiTextureType_LIGHTMAP;
		}
	}
}