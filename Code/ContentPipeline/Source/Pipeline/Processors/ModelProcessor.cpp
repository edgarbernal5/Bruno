#include "ModelProcessor.h"

#include "Pipeline/Graphics/ModelContentItem.h"
#include "Pipeline/Graphics/MaterialContentItem.h"
#include "Pipeline/ExternalReferenceContentItem.h"

#include <Bruno/Platform/DirectX/D3DCommon.h>
#include <filesystem>
#include <sstream>

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelProcessor);

	std::map<ModelProcessor::TextureType, std::pair<uint32_t, std::string>> ModelProcessor::g_textureTypeMappings;

	ModelProcessor::ModelProcessor()
	{
		m_supportedExtensions = { L".fbx", L".x" };

		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeDiffuse] = std::make_pair(aiTextureType_DIFFUSE, "Texture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeSpecularMap] = std::make_pair(aiTextureType_SPECULAR, "SpecularTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeAmbient] = std::make_pair(aiTextureType_AMBIENT, "AmbientTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeEmissive] = std::make_pair(aiTextureType_EMISSIVE, "EmissiveTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeHeightmap] = std::make_pair(aiTextureType_HEIGHT, "HeightTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeNormalMap] = std::make_pair(aiTextureType_NORMALS, "NormalTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeSpecularPowerMap] = std::make_pair(aiTextureType_SHININESS, "SpecularPowerTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeDisplacementMap] = std::make_pair(aiTextureType_DISPLACEMENT, "DisplacementTexture");
		g_textureTypeMappings[ModelProcessor::TextureType::TextureTypeLightMap] = std::make_pair(aiTextureType_LIGHTMAP, "LightMapTexture");
	}

	std::shared_ptr<ContentItem> ModelProcessor::Process(const std::wstring& assetFilename, ContentProcessorContext& context)
	{
		Assimp::Importer importer;

		uint32_t flags = aiProcess_Triangulate |
			aiProcess_OptimizeMeshes |
			aiProcess_ImproveCacheLocality |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_SortByPType | 
			aiProcess_FindInvalidData |
			aiProcess_FindDegenerates |
			aiProcess_FlipWindingOrder |
			aiProcess_FlipUVs;

		importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, true);

		const aiScene* aiScene = importer.ReadFile(std::string(assetFilename.begin(), assetFilename.end()), flags);
		if (aiScene == nullptr)
		{
			throw std::exception();
		}

		std::vector<std::shared_ptr<MaterialContentItem>> materials;
		if (aiScene->HasMaterials())
		{
			std::filesystem::path directory = assetFilename;

			ProcessMaterials(aiScene, directory.parent_path(), materials, context);
		}

		std::vector<std::shared_ptr<MeshContentItem>> meshes;
		if (aiScene->HasMeshes())
		{
			for (size_t i = 0; i < aiScene->mNumMeshes; i++)
			{
				ProcessMesh(aiScene->mMeshes[i], meshes);
			}
		}

		auto model = std::make_shared<ModelContentItem>(std::move(materials), std::move(meshes));
		return model;
	}

	void ModelProcessor::ProcessMesh(aiMesh* aiMesh, std::vector<std::shared_ptr<MeshContentItem>>& meshes)
	{
		auto mesh = std::make_shared<MeshContentItem>();
		mesh->MaterialIndex = aiMesh->mMaterialIndex;
		mesh->Vertices.reserve(aiMesh->mNumVertices);
		for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
		{
			mesh->Vertices.emplace_back(reinterpret_cast<const float*>(&aiMesh->mVertices[i]));
		}

		if (aiMesh->HasNormals())
		{
			mesh->Normals.reserve(aiMesh->mNumVertices);
			for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
			{
				mesh->Normals.emplace_back(reinterpret_cast<const float*>(&aiMesh->mNormals[i]));
			}
		}

		if (aiMesh->HasTangentsAndBitangents())
		{
			mesh->Tangents.reserve(aiMesh->mNumVertices);
			mesh->BiNormals.reserve(aiMesh->mNumVertices);
			for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
			{
				mesh->Tangents.emplace_back(reinterpret_cast<const float*>(&aiMesh->mTangents[i]));
				mesh->BiNormals.emplace_back(reinterpret_cast<const float*>(&aiMesh->mBitangents[i]));
			}
		}

		uint32_t uvChannelCount = aiMesh->GetNumUVChannels();
		mesh->TextureCoordinates.reserve(uvChannelCount);
		for (uint32_t i = 0; i < uvChannelCount; i++)
		{
			std::vector<Math::Vector3> textureCoordinates;
			textureCoordinates.reserve(aiMesh->mNumVertices);
			aiVector3D* aiTextureCoordinates = aiMesh->mTextureCoords[i];
			for (uint32_t j = 0; j < aiMesh->mNumVertices; j++)
			{
				textureCoordinates.emplace_back(reinterpret_cast<const float*>(&aiTextureCoordinates[j]));
			}

			mesh->TextureCoordinates.push_back(std::move(textureCoordinates));
		}

		uint32_t colorChannelCount = aiMesh->GetNumColorChannels();
		mesh->VertexColors.reserve(colorChannelCount);
		for (uint32_t i = 0; i < colorChannelCount; i++)
		{
			std::vector<Math::Vector4> vertexColors;
			vertexColors.reserve(aiMesh->mNumVertices);
			aiColor4D* aiVertexColors = aiMesh->mColors[i];
			for (uint32_t j = 0; j < aiMesh->mNumVertices; j++)
			{
				vertexColors.emplace_back(reinterpret_cast<const float*>(&aiVertexColors[j]));
			}
			mesh->VertexColors.push_back(std::move(vertexColors));
		}

		if (aiMesh->HasFaces())
		{
			mesh->FaceCount = aiMesh->mNumFaces;
			for (uint32_t i = 0; i < mesh->FaceCount; i++)
			{
				aiFace* face = &aiMesh->mFaces[i];

				for (uint32_t j = 0; j < face->mNumIndices; j++)
				{
					mesh->Indices.push_back(face->mIndices[j]);
				}
			}
		}

		meshes.emplace_back(mesh);
	}

	void ModelProcessor::ProcessMaterials(const aiScene* aiScene, const std::wstring& directory, std::vector<std::shared_ptr<MaterialContentItem>>& materials, ContentProcessorContext& context)
	{
		for (uint32_t i = 0; i < aiScene->mNumMaterials; i++)
		{
			aiMaterial* aiMaterial = aiScene->mMaterials[i];
			auto materialContent = std::make_shared<MaterialContentItem>();

			aiString name;
			aiMaterial->Get(AI_MATKEY_NAME, name);
			materialContent->Name = name.C_Str();

			ProcessTexturesForMaterial(*materialContent, aiMaterial, directory, context);

			materials.emplace_back(std::move(materialContent));
		}
	}

	void ModelProcessor::ProcessTexturesForMaterial(MaterialContentItem& materialContentItem, aiMaterial* aiMaterial, const std::wstring& directory, ContentProcessorContext& context)
	{
		for (auto it = g_textureTypeMappings.begin(); it != g_textureTypeMappings.end(); ++it)
		{
			aiTextureType mappedTextureType = static_cast<aiTextureType>(it->second.first);
			uint32_t textureCount = aiMaterial->GetTextureCount(mappedTextureType);

			for (uint32_t textureIndex = 0; textureIndex < textureCount; textureIndex++)
			{
				aiString textureRelativePath;
				aiTextureMapping mapping = aiTextureMapping::aiTextureMapping_UV;
				uint32_t uvIndex = 0;
				float blend = 0.0f;
				aiTextureOp operation = aiTextureOp::aiTextureOp_Multiply;
				aiTextureMapMode mapMode = aiTextureMapMode::aiTextureMapMode_Wrap;

				if (aiMaterial->GetTexture(mappedTextureType, textureIndex, &textureRelativePath, &mapping, &uvIndex, &blend, &operation, &mapMode) == AI_SUCCESS)
				{
					std::filesystem::path filenameTexture(directory);
					filenameTexture /= textureRelativePath.C_Str();

					if (std::filesystem::exists(filenameTexture))
					{
						std::wstringstream relativePathToTextureW;
						relativePathToTextureW << textureRelativePath.C_Str();

						ExternalReferenceContentItem sourceTextureReference(filenameTexture);
						ExternalReferenceContentItem textureReference = context.BuildAsset(sourceTextureReference, "TextureProcessor", L"");
						std::string textureName = (it->second.second);

						materialContentItem.AddTexture(textureName, textureReference);
					}
				}
			}
		}
	}
}