#include "brpch.h"
#include "ModelImporter.h"

#include "Bruno/Core/StringHelpers.h"
#include "Bruno/Renderer/Material.h"

namespace Bruno
{
	std::map<ModelImporter::TextureType, std::pair<uint32_t, std::string>> ModelImporter::g_textureTypeMappings;

	ModelImporter::ModelImporter()
	{
		if (g_textureTypeMappings.size() > 0)
			return;

		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDiffuse] = std::make_pair(aiTextureType_DIFFUSE, "Texture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularMap] = std::make_pair(aiTextureType_SPECULAR, "SpecularTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeAmbient] = std::make_pair(aiTextureType_AMBIENT, "AmbientTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeEmissive] = std::make_pair(aiTextureType_EMISSIVE, "EmissiveTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeHeightmap] = std::make_pair(aiTextureType_HEIGHT, "HeightTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeNormalMap] = std::make_pair(aiTextureType_NORMALS, "NormalTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeSpecularPowerMap] = std::make_pair(aiTextureType_SHININESS, "SpecularPowerTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeDisplacementMap] = std::make_pair(aiTextureType_DISPLACEMENT, "DisplacementTexture");
		g_textureTypeMappings[ModelImporter::TextureType::TextureTypeLightMap] = std::make_pair(aiTextureType_LIGHTMAP, "LightMapTexture");
	}

	bool ModelImporter::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset)
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

		auto assetFilename = context.GetAbsolutePath(metadata.Filename);
		const aiScene* aiScene = importer.ReadFile(WStringToString(assetFilename), flags);
		if (aiScene == nullptr)
		{
			return false;
		}

		std::vector<std::shared_ptr<Material>> materials;
		if (aiScene->HasMaterials())
		{
			std::filesystem::path directory = assetFilename;

			ProcessMaterials(aiScene, directory.parent_path(), materials, context);
		}

		std::vector<ModelVertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<std::shared_ptr<Mesh>> meshes;
		if (aiScene->HasMeshes())
		{
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			for (size_t i = 0; i < aiScene->mNumMeshes; i++)
			{
				ProcessMesh(aiScene->mMeshes[i], vertices, indices, meshes, vertexCount, indexCount);
			}
		}

		std::vector<ModelNode> modelNodes;
		auto& rootNode = modelNodes.emplace_back();
		rootNode.Parent = ModelNode::NullNode;
		ProcessNode(aiScene->mRootNode, 0, modelNodes, meshes, Math::Matrix::Identity);

		asset = std::make_shared<Model>(std::move(vertices), std::move(indices), std::move(materials), std::move(meshes), std::move(modelNodes));
		asset->SetHandle(metadata.Handle);

		return true;
	}

	void ModelImporter::ProcessMesh(aiMesh* aiMesh, std::vector<ModelVertex>& vertices, std::vector<uint32_t>& indices, std::vector<std::shared_ptr<Mesh>>& meshes, uint32_t& baseVertex, uint32_t& baseIndex)
	{
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
				vertex.Texcoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y, aiMesh->mTextureCoords[0][i].z };
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
				BR_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "Must have 3 indices!");

				uint32_t index0 = aiMesh->mFaces[i].mIndices[0];
				uint32_t index1 = aiMesh->mFaces[i].mIndices[1];
				uint32_t index2 = aiMesh->mFaces[i].mIndices[2];

				indices.push_back(index0);
				indices.push_back(index1);
				indices.push_back(index2);
			}
		}

		Math::BoundingBox aabb;
		aabb.Center = (aabbMax + aabbMin) * 0.5f;
		aabb.Extents = (aabbMax - aabbMin) * 0.5f;

		auto mesh = std::make_shared<Mesh>(aiMesh->mName.C_Str(), baseVertex, baseIndex, aiMesh->mNumVertices, aiMesh->mNumFaces * 3, aiMesh->mMaterialIndex, Math::Matrix::Identity, Math::Matrix::Identity, aabb);

		baseVertex += aiMesh->mNumVertices;
		baseIndex += aiMesh->mNumFaces * 3;

		meshes.emplace_back(mesh);
	}

	void ModelImporter::ProcessNode(aiNode* aiNode, uint32_t nodeIndex, std::vector<ModelNode>& modelNodes, std::vector<std::shared_ptr<Mesh>>& meshes, const Math::Matrix& parentTransform)
	{
		auto& node = modelNodes[nodeIndex];
		node.Name = aiNode->mName.C_Str();
		node.LocalTransform = ToMatrix(aiNode->mTransformation);

		Math::Matrix transform = parentTransform * node.LocalTransform;
		for (uint32_t i = 0; i < aiNode->mNumMeshes; i++)
		{
			uint32_t submeshIndex = aiNode->mMeshes[i];
			auto& submesh = meshes[submeshIndex];
			submesh->SetNodeName(aiNode->mName.C_Str());
			submesh->SetTransform(transform);
			submesh->SetLocalTransform(node.LocalTransform);

			node.Meshes.push_back(submeshIndex);
		}

		uint32_t parentNodeIndex = (uint32_t)modelNodes.size() - 1;
		node.Children.resize(aiNode->mNumChildren);
		for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
		{
			auto& child = modelNodes.emplace_back();

			uint32_t childIndex = static_cast<uint32_t>(modelNodes.size()) - 1;
			child.Parent = parentNodeIndex;
			modelNodes[nodeIndex].Children[i] = childIndex;
			ProcessNode(aiNode->mChildren[i], childIndex, modelNodes, meshes, transform);
		}
	}

	void ModelImporter::ProcessMaterials(const aiScene* aiScene, const std::wstring& directory, std::vector<std::shared_ptr<Material>>& materials, AssetImporterContext& context)
	{
		for (uint32_t i = 0; i < aiScene->mNumMaterials; i++)
		{
			aiMaterial* aiMaterial = aiScene->mMaterials[i];
			auto materialContent = std::make_shared<Material>();

			aiString name;
			aiMaterial->Get(AI_MATKEY_NAME, name);
			materialContent->Name = name.C_Str();

			ProcessTexturesForMaterial(*materialContent, aiMaterial, directory, context);

			context.AddMemoryOnlyAsset(materialContent);//TODO: HACK

			materials.emplace_back(materialContent);
		}
	}

	void ModelImporter::ProcessTexturesForMaterial(Material& materialContentItem, aiMaterial* aiMaterial, const std::wstring& directory, AssetImporterContext& context)
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
						std::string textureName = (it->second.second);

						auto textureHandle = context.ImportAsset(filenameTexture);
						materialContentItem.TexturesByName[textureName] = textureHandle;
					}
				}
			}
		}
	}

	Math::Matrix ModelImporter::ToMatrix(const aiMatrix4x4& aiMatrix)
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
}