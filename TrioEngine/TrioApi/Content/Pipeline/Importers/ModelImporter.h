#pragma once

#include "TrioApiRequisites.h"

#include "IContentImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace TrioEngine
{
	class NodeContent;
	class MeshContent;
	class MaterialContent;

	class TRIO_API_EXPORT ModelImporter : public IContentImporter
	{
	public:
		ModelImporter();

		ContentItem* Import(std::string& filename);

		friend class BuildCoordinator;

		std::string GetImporterName() const { return "ModelImporter"; }
		std::string GetFileExtensions() const { return "x, obj, fbx"; }

	private:
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

		MeshContent* CreateMesh(aiMesh* mesh);
		void GetTexturesForMaterial(MaterialContent* materialContent, std::string& directory, aiMaterial* material);
		void ImportMaterials(const aiScene* scene, std::string directory);
		NodeContent* ProcessNode(const aiScene* scene, const aiNode* node, Matrix parentTransform);

		std::vector<MaterialContent*> m_materials;

		static void InitializeTextureTypeMappings();
		static std::map<TextureType, uint32_t> g_textureTypeMappings;
		static std::map<TextureType, std::string> g_textureNameMappings;
	};
}