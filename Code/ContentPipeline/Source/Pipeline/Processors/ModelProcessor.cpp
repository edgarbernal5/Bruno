#include "ModelProcessor.h"

#include "Pipeline/Graphics/ModelContentItem.h"
#include "Pipeline/Graphics/MaterialContentItem.h"
#include "Pipeline/ExternalReferenceContentItem.h"

#include <filesystem>
#include <sstream>
#include <Bruno/Core/StringHelpers.h>

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
			aiProcess_ConvertToLeftHanded |
			aiProcess_FlipUVs;

		//TODO: sort by primitive type (i.e. aiProcess_SortByPType)
		//Look at AssimpMeshImporter.cpp in Hazel
		//aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded

		importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, true);

		const aiScene* aiScene = importer.ReadFile(WStringToString(assetFilename), flags);
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

		std::vector<ModelVertexContentItem> vertices;
		std::vector<uint32_t> indices;
		std::vector<std::shared_ptr<MeshContentItem>> meshes;
		if (aiScene->HasMeshes())
		{
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			for (size_t i = 0; i < aiScene->mNumMeshes; i++)
			{
				ProcessMesh(aiScene->mMeshes[i], vertices, indices, meshes, vertexCount, indexCount);
			}
		}

		std::vector<std::shared_ptr<ModelNodeContentItem>> modelNodes;
		auto rootNode = std::make_shared<ModelNodeContentItem>();
		modelNodes.emplace_back(rootNode);
		ProcessNode(aiScene->mRootNode, 0, modelNodes, meshes, Math::Matrix::Identity);

		auto model = std::make_shared<ModelContentItem>(std::move(vertices), std::move(indices), std::move(materials), std::move(meshes), std::move(modelNodes));
		return model;
	}

	void ModelProcessor::ProcessMesh(aiMesh* aiMesh, std::vector<ModelVertexContentItem>& vertices, std::vector<uint32_t>& indices, std::vector<std::shared_ptr<MeshContentItem>>& meshes, uint32_t& baseVertex, uint32_t& baseIndex)
	{
		auto mesh = std::make_shared<MeshContentItem>();
		mesh->MeshName = aiMesh->mName.C_Str();
		mesh->BaseVertex = baseVertex;
		mesh->BaseIndex = baseIndex;
		mesh->VertexCount = aiMesh->mNumVertices;
		mesh->IndexCount = aiMesh->mNumFaces * 3;
		mesh->MaterialIndex = aiMesh->mMaterialIndex;
		//vertices.reserve(aiMesh->mNumVertices);

		Math::Vector3 aabbMin = { FLT_MAX, FLT_MAX, FLT_MAX };
		Math::Vector3 aabbMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
		{
			auto& vertex = vertices.emplace_back();
			vertex.Position = { aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z };

			aabbMin.x = std::min<float>(aabbMin.x, vertex.Position.x);
			aabbMin.y = std::min<float>(aabbMin.y, vertex.Position.y);
			aabbMin.z = std::min<float>(aabbMin.z, vertex.Position.z);
			aabbMax.x = std::max<float>(aabbMax.x, vertex.Position.x);
			aabbMax.y = std::max<float>(aabbMax.y, vertex.Position.y);
			aabbMax.z = std::max<float>(aabbMax.z, vertex.Position.z);

			if (aiMesh->HasNormals())
				vertex.Normal = { aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z };

			if (aiMesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z };
				vertex.Binormal = { aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z };
			}

			if (aiMesh->HasTextureCoords(0))
				vertex.Texcoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y,0.0f };
		}

		/*uint32_t colorChannelCount = aiMesh->GetNumColorChannels();
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
		}*/

		if (aiMesh->HasFaces())
		{
			for (uint32_t i = 0; i < aiMesh->mNumFaces; i++)
			{
				aiFace* face = &aiMesh->mFaces[i];

				for (uint32_t j = 0; j < face->mNumIndices; j++)
				{
					indices.push_back(face->mIndices[j]);
				}
			}
		}

		auto& aabb = mesh->BBox;
		aabb.Center = (aabbMax + aabbMin) * 0.5f;
		aabb.Extents = (aabbMax - aabbMin) * 0.5f;

		baseVertex += aiMesh->mNumVertices;
		baseIndex += mesh->IndexCount;

		meshes.emplace_back(mesh);
	}

	void ModelProcessor::ProcessNode(aiNode* aiNode, uint32_t nodeIndex, std::vector<std::shared_ptr<ModelNodeContentItem>>& modelNodes, std::vector<std::shared_ptr<MeshContentItem>>& meshes, const Math::Matrix& parentTransform)
	{
		auto node = modelNodes[nodeIndex].get();
		node->Name = aiNode->mName.C_Str();
		node->LocalTransform = ToMatrix(aiNode->mTransformation);

		Math::Matrix transform = parentTransform * node->LocalTransform;
		for (uint32_t i = 0; i < aiNode->mNumMeshes; i++)
		{
			uint32_t submeshIndex = aiNode->mMeshes[i];
			auto& submesh = meshes[submeshIndex];
			submesh->NodeName = aiNode->mName.C_Str();
			submesh->Transform = transform;
			submesh->LocalTransform = node->LocalTransform;

			node->Meshes.push_back(submeshIndex);
		}

		uint32_t parentNodeIndex = (uint32_t)modelNodes.size() - 1;
		node->Children.resize(aiNode->mNumChildren);
		for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
		{
			auto child = std::make_shared<ModelNodeContentItem>();
			modelNodes.emplace_back(child);

			uint32_t childIndex = static_cast<uint32_t>(modelNodes.size()) - 1;
			child->Parent = parentNodeIndex;
			modelNodes[nodeIndex]->Children[i] = childIndex;
			ProcessNode(aiNode->mChildren[i], childIndex, modelNodes, meshes, transform);
		}
	}

	Math::Matrix ModelProcessor::ToMatrix(const aiMatrix4x4& aiMatrix)
	{
		Math::Matrix transform;
		transform._11 = aiMatrix.a1;
		transform._12 = aiMatrix.b1;
		transform._13 = aiMatrix.c1;
		transform._14 = aiMatrix.d1;

		transform._21 = aiMatrix.a2;
		transform._22 = aiMatrix.b2;
		transform._23 = aiMatrix.c2;
		transform._24 = aiMatrix.d2;

		transform._31 = aiMatrix.a3;
		transform._32 = aiMatrix.b3;
		transform._33 = aiMatrix.c3;
		transform._34 = aiMatrix.d3;

		transform._41 = aiMatrix.a4;
		transform._42 = aiMatrix.b4;
		transform._43 = aiMatrix.c4;
		transform._44 = aiMatrix.d4;

		return transform;
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