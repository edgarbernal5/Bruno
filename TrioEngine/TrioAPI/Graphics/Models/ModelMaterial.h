#pragma once

#include "TrioApiRequisites.h"

#include <string>

struct aiMaterial;

namespace TrioEngine
{	
	class TRIO_API_EXPORT ModelMaterial
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

		inline const char* GetName() { return m_name.c_str(); }
		const std::map<TextureType, std::vector<std::wstring>*>& GetTextures() const;

		friend class Model;
	private:
		ModelMaterial(aiMaterial* material);

		static std::map<ModelMaterial::TextureType, uint32_t> g_textureTypeMappings;
		static void InitializeTextureTypeMappings();

		std::string m_name;
		std::map<TextureType, std::vector<std::wstring>*> m_textures;
	};
}