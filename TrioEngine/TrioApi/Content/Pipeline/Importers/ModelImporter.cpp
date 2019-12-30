#include "stdafx.h"
#include "ModelImporter.h"

#include "Content/Pipeline/Graphics/GeometryContent.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"
#include "Content/Pipeline/Graphics/MeshContent.h"
#include "IO/File.h"

namespace TrioEngine
{
	std::map<ModelImporter::TextureType, uint32_t> ModelImporter::g_textureTypeMappings;
	std::map<ModelImporter::TextureType, std::string> ModelImporter::g_textureNameMappings;

	ModelImporter::ModelImporter()
	{
	}

	MeshContent* ModelImporter::CreateMesh(aiMesh* mesh)
	{
		MeshContent* meshContent = new MeshContent();
		meshContent->SetName(mesh->mName.C_Str());

		for (uint32_t v = 0; v < mesh->mNumVertices; v++)
		{
			meshContent->GetPositions().push_back(Vector3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
		}

		GeometryContent* geom = new GeometryContent();

		return meshContent;
	}

	void ModelImporter::GetTexturesForMaterial(MaterialContent* materialContent, std::string& directory, aiMaterial* material)
	{
		//TODO: terminar las demás propiedades.
		for (TextureType textureType = (TextureType)0; textureType < ModelImporter::TextureType::TextureTypeEnd; textureType = (TextureType)(textureType + 1))
		{
			aiTextureType mappedTextureType = (aiTextureType)g_textureTypeMappings[textureType];
			uint32_t textureCount = material->GetTextureCount(mappedTextureType);

			if (textureCount > 0)
			{
				for (uint32_t textureIndex = 0; textureIndex < textureCount; textureIndex++)
				{
					aiString textureRelativePath;
					aiTextureMapping mapping = aiTextureMapping::aiTextureMapping_UV;
					unsigned int uvIndex = 0;
					float blend = 0.0f;

					if (material->GetTexture(mappedTextureType, textureIndex, &textureRelativePath, &mapping, &uvIndex, &blend) == AI_SUCCESS)
					{
						std::string filenameTexture = TrioIO::Path::Combine(directory, textureRelativePath.C_Str());
						if (TrioIO::File::Exist(filenameTexture))
						{
							ExternalReference texture(filenameTexture);

							std::stringstream ss;
							ss << "TextureCoordinate" << uvIndex;
							texture.GetOpaqueData()["TextureCoordinate"] = ss.str();

							materialContent->GetTextures()[g_textureNameMappings[textureType]] = texture;
						}
					}
				}
			}
		}
	}

	void ModelImporter::InitializeTextureTypeMappings()
	{
		if (g_textureTypeMappings.size() != ModelImporter::TextureType::TextureTypeEnd)
		{
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDifffuse] = aiTextureType_DIFFUSE;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularMap] = aiTextureType_SPECULAR;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeAmbient] = aiTextureType_AMBIENT;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeHeightmap] = aiTextureType_HEIGHT;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeNormalMap] = aiTextureType_NORMALS;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
			g_textureTypeMappings[ModelImporter::TextureType::TextureTypeLightMap] = aiTextureType_LIGHTMAP;

			g_textureNameMappings[ModelImporter::TextureType::TextureTypeDifffuse] = "Texture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeSpecularMap] = "SpecularTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeAmbient] = "AmbientTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeHeightmap] = "HeightTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeNormalMap] = "NormalTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeSpecularPowerMap] = "SpecularPowerTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeDisplacementMap] = "DisplacementTexture";
			g_textureNameMappings[ModelImporter::TextureType::TextureTypeLightMap] = "LightMapTexture";
		}
	}

	ContentItem* ModelImporter::Import(std::string& filename)
	{
		Assimp::Importer importer;

		UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		//if (loadParams.flipUVs)
		//{
		//	flags |= aiProcess_FlipUVs;
		//}

		const aiScene* scene = importer.ReadFile(filename, flags);
		if (scene == nullptr)
		{
			//throw GameException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			ImportMaterials(scene, TrioIO::Path::GetDirectoryFromFilePath(filename));
		}
		NodeContent* rootNode = ProcessNode(scene, scene->mRootNode, Matrix::Identity);

		if (scene->HasMeshes())
		{
			for (uint32_t i = 0; i < scene->mNumMeshes; i++)
			{
				/*Mesh* mesh = new Mesh(*this, *(scene->mMeshes[i]));
				m_meshes.push_back(mesh);*/
			}
		}

		if (scene->HasAnimations())
		{
			/*assert(scene->mRootNode != nullptr);
			m_pRootNode = BuildSkeleton(*scene->mRootNode, nullptr);

			mAnimations.reserve(scene->mNumAnimations);
			for (UINT i = 0; i < scene->mNumAnimations; i++)
			{
				AnimationClip* animationClip = new AnimationClip(*this, *(scene->mAnimations[i]));
				mAnimations.push_back(animationClip);
				mAnimationsByName.insert(std::pair<std::string, AnimationClip*>(animationClip->Name(), animationClip));
			}*/
		}
		return rootNode;
	}

	void ModelImporter::ImportMaterials(const aiScene* scene, std::string directory)
	{
		m_materials.clear();
		for (UINT i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* material = scene->mMaterials[i];
			MaterialContent* materialContent = new MaterialContent();
			
			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			materialContent->SetName(name.C_Str());

			GetTexturesForMaterial(materialContent, directory, material);
			
			m_materials.push_back(materialContent);
		}
	}

	NodeContent* ModelImporter::ProcessNode(const aiScene* scene, const aiNode* node, Matrix parentTransform) {
		NodeContent* nodeContent = nullptr;

		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			if (mesh->mNumVertices == 0)
				continue;
		}

		return nodeContent;
	}
}