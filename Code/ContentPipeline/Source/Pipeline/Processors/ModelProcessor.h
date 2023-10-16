#pragma once

#include "Pipeline/AbstractProcessor.h"

#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Bruno
{
	struct MaterialContentItem;
	struct MeshContentItem;

	class ModelProcessor : public AbstractProcessor
	{
		BR_RTTI_DECLARATION(ModelProcessor, AbstractProcessor);

	public:
		ModelProcessor();

		virtual std::shared_ptr<ContentItem> Process(const std::wstring& assetFilename, ContentProcessorContext& context) override;

	private:
		enum TextureType : uint32_t
		{
			TextureTypeDiffuse = 0,
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

		void ProcessMesh(aiMesh* aiMesh, std::vector<std::shared_ptr<MeshContentItem>>& meshes);
		void ProcessMaterials(const aiScene* aiScene, const std::wstring& directory, std::vector<std::shared_ptr<MaterialContentItem>>& materials, ContentProcessorContext& context);
		void ProcessTexturesForMaterial(MaterialContentItem& materialContentItem, aiMaterial* aiMaterial, const std::wstring& directory, ContentProcessorContext& context);

		static std::map<TextureType, std::pair<uint32_t, std::string>> g_textureTypeMappings;
	};
}
