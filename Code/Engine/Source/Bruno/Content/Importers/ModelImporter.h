#pragma once

#include "Bruno/Content/AssetImporter.h"
#include "Bruno/Math/Math.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Bruno
{
	class ModelImporter : public AbstractAssetImporter
	{
	public:
		ModelImporter();
		bool TryImport(const AssetMetadata& metadata, std::shared_ptr<Asset>& asset) override;

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

		/*void ProcessMesh(aiMesh* aiMesh, std::vector<ModelVertexContentItem>& vertices, std::vector<uint32_t>& indices, std::vector<std::shared_ptr<MeshContentItem>>& meshes, uint32_t& baseVertex, uint32_t& baseIndex);
		void ProcessNode(aiNode* aiNode, uint32_t nodeIndex, std::vector<std::shared_ptr<ModelNodeContentItem>>& modelNodes, std::vector<std::shared_ptr<MeshContentItem>>& meshes, const Math::Matrix& parentTransform);
		void ProcessMaterials(const aiScene* aiScene, const std::wstring& directory, std::vector<std::shared_ptr<MaterialContentItem>>& materials, ContentProcessorContext& context);
		void ProcessTexturesForMaterial(MaterialContentItem& materialContentItem, aiMaterial* aiMaterial, const std::wstring& directory, ContentProcessorContext& context);
		Math::Matrix ToMatrix(const aiMatrix4x4& aiMatrix);*/

		static std::map<TextureType, std::pair<uint32_t, std::string>> g_textureTypeMappings;
	};
}

