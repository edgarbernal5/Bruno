#pragma once

#include "TrioApiDecl.h"

#include <string>

struct aiMaterial;

namespace TrioEngine
{	
	class TRIOAPI_DECL ModelMaterial
	{
	public:
		ModelMaterial();
		~ModelMaterial();

		enum TextureType : uint32_t
		{
			TextureTypeDifffuse = 0,
			TextureTypeSpecularMap,
			TextureTypeAmbient,
			TextureTypeEmissive,
			TextureTypeHeightmap,
			TextureTypeNormalMap,
			TextureTypeSpecularPowerMap,
			TextureTypeDisplacementMap,
			TextureTypeLightMap,
			TextureTypeEnd
		};

		friend class Model;
	private:
		ModelMaterial(aiMaterial* material);

		static std::map<ModelMaterial::TextureType, uint32_t> g_mTextureTypeMappings;
		static void InitializeTextureTypeMappings();

		std::string m_csName;
		std::map<TextureType, std::vector<std::wstring>*> m_mTextures;
	};
}