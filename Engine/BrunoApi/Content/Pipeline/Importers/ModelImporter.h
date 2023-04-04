#pragma once

#include "TrioApiRequisites.h"

#include "IContentImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Content/Pipeline/ContentIdentity.h"

#include <utility>
#include <string>

namespace BrunoEngine
{
	class NodeContent;
	class MeshContent;
	class MaterialContent;
	class GeometryContent;

	class BRUNO_API_EXPORT ModelImporter : public IContentImporter
	{
	public:
		ModelImporter();

		ContentItem* Import(const std::string& filename);

		friend class GameContentBuilder;

		std::string GetImporterName() const { return "ModelImporter"; }
		std::string GetFileExtensions() const { return "x, obj, fbx"; }

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

		ContentIdentity m_identity;

		GeometryContent* CreateGeometry(aiMesh* mesh, MeshContent* meshContent);

		void GetTexturesForMaterial(MaterialContent* materialContent, std::string& directory, aiMaterial* material);
		void ImportMaterials(const aiScene* scene, std::string directory);
		NodeContent* ImportNode(const aiScene* scene, const aiNode* currentNode, const aiNode* parentNode, NodeContent* parentContent);

		Matrix GetRelativeTransform(const aiNode* node, const aiNode* ancestor);
		Matrix ToMatrix(const aiMatrix4x4& aiMatrix);
		std::vector<MaterialContent*> m_materials;

		static void InitializeTextureTypeMappings();

		static std::map<TextureType, std::pair<uint32_t, std::string>> g_textureTypeMappings;
		//static std::map<TextureType, std::string> g_textureNameMappings;
	};
}