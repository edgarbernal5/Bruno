#include "stdafx.h"
#include "ModelImporter.h"

#include "Content/Pipeline/Graphics/GeometryContent.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"
#include "Content/Pipeline/Graphics/MeshContent.h"
#include "Content/Pipeline/Graphics/VertexContent.h"
#include "Content/Pipeline/Graphics/VertexChannelCollection.h"

#include "Content/Pipeline/Graphics/VertexChannelNames.h"
#include "IO/File.h"

#include "Errors/ContentPipelineException.h"

namespace BrunoEngine
{
	std::map<ModelImporter::TextureType, std::pair<uint32_t, std::string>> ModelImporter::g_textureTypeMappings;

	ModelImporter::ModelImporter()
	{
		InitializeTextureTypeMappings();
	}

	GeometryContent* ModelImporter::CreateGeometry(aiMesh* mesh, MeshContent* meshContent)
	{
		GeometryContent* geometryContent = new GeometryContent();
		geometryContent->SetContentIdentity(m_identity);
		geometryContent->SetMaterial(m_materials[mesh->mMaterialIndex]);

		int baseVertex = meshContent->GetPositions().size();
		for (uint32_t v = 0; v < mesh->mNumVertices; v++)
		{
			meshContent->GetPositions().push_back(Vector3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
		}

		std::vector<int> vertexRange(mesh->mNumVertices);
		for (int i = 0; i < mesh->mNumVertices; i++) vertexRange[i] = i + baseVertex;

		geometryContent->GetVertices()->AddRange(vertexRange.data(), mesh->mNumVertices);

		if (mesh->HasFaces())
		{
			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace* face = &mesh->mFaces[i];

				for (uint32_t j = 0; j < face->mNumIndices; j++)
				{
					geometryContent->GetIndices().push_back(face->mIndices[j]);
				}
			}
		}

		if (mesh->HasBones())
		{
			//TODO
		}

		if (mesh->HasNormals())
		{
			std::vector<Vector3> normals;
			normals.reserve(mesh->mNumVertices);
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				normals.push_back(Vector3(reinterpret_cast<const float*>(&mesh->mNormals[i])));
			}

			geometryContent->GetVertices()->GetChannels()->Add<Vector3>(VertexChannelNames::Normal(), normals.data(), mesh->mNumVertices);
		}

		uint32_t uvChannelCount = mesh->GetNumUVChannels();
		for (uint32_t i = 0; i < uvChannelCount; i++)
		{
			std::vector<Vector2> textureCoordinates(mesh->mNumVertices);
			
			aiVector3D* aiTextureCoordinates = mesh->mTextureCoords[i];
			for (uint32_t j = 0; j < mesh->mNumVertices; j++)
			{
				textureCoordinates[j] = Vector2(reinterpret_cast<const float*>(&aiTextureCoordinates[j]));
			}

			geometryContent->GetVertices()->GetChannels()->Add<Vector2>(VertexChannelNames::TextureCoordinate(i), textureCoordinates.data(), mesh->mNumVertices);
		}

		uint32_t colorChannelCount = mesh->GetNumColorChannels();
		for (uint32_t i = 0; i < colorChannelCount; i++)
		{
			std::vector<Color> vertexColors(mesh->mNumVertices);

			aiColor4D* aiVertexColors = mesh->mColors[i];
			for (uint32_t j = 0; j < mesh->mNumVertices; j++)
			{
				vertexColors[j] = Color(reinterpret_cast<const float*>(&aiVertexColors[j]));
			}

			geometryContent->GetVertices()->GetChannels()->Add<Color>(VertexChannelNames::Color(i), vertexColors.data(), mesh->mNumVertices);
		}

		geometryContent->SetParent(meshContent);
		return geometryContent;
	}

	void ModelImporter::GetTexturesForMaterial(MaterialContent* materialContent, std::string& directory, aiMaterial* aiMaterial)
	{
		//TODO: terminar las demás propiedades.
		for (auto it = g_textureTypeMappings.begin(); it != g_textureTypeMappings.end(); ++it)
		{
			aiTextureType mappedTextureType = (aiTextureType)(it->second.first);
			uint32_t textureCount = aiMaterial->GetTextureCount(mappedTextureType);

			if (textureCount > 0)
			{
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
						std::string filenameTexture = BrunoIO::Path::Combine(directory, textureRelativePath.C_Str());
						if (BrunoIO::File::Exist(filenameTexture))
						{
							ExternalReference texture(filenameTexture);
							texture.SetContentIdentity(m_identity);

							std::string textureName = (it->second.second);

							std::stringstream ss;
							ss << "TextureCoordinate" << uvIndex;
							texture.GetOpaqueData().set("TextureCoordinate", ss.str());
							//texture.GetOpaqueData()["AddressU"] = aiTextureMapMode
							//Operation
							//AddressU
							//AddressV
							//Mapping

							materialContent->GetTextures()[textureName] = texture;
						}
					}

					aiColor3D color(1.0f, 1.0f, 1.0f);
					aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

					materialContent->GetOpaqueData().set("DiffuseColor", color);
				}
			}
		}
	}

	void ModelImporter::InitializeTextureTypeMappings()
	{
		if (g_textureTypeMappings.size() == 0)
		{
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
	}

	ContentItem* ModelImporter::Import(const std::string& filename)
	{
		m_identity = ContentIdentity(filename, GetImporterName());
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

		const aiScene* scene = importer.ReadFile(filename, flags);
		if (scene == nullptr)
		{
			throw ContentPipelineException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			ImportMaterials(scene, BrunoIO::Path::GetDirectoryFromFilePath(filename));
		}

		std::map<std::string, Matrix> deformationBones;
		if (scene->HasMeshes()) {
			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[i];
				if (mesh->HasBones()) {
					for (size_t j = 0; j < mesh->mNumBones; j++)
					{
						aiBone* bone = mesh->mBones[j];
						std::string boneName(bone->mName.C_Str());
						if (deformationBones.find(boneName) == deformationBones.end()) {
							deformationBones[boneName] = ToMatrix(bone->mOffsetMatrix);
						}
					}
				}
			}
		}

		NodeContent* rootNode = ImportNode(scene, scene->mRootNode, nullptr, nullptr);

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
		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aiMaterial = scene->mMaterials[i];
			MaterialContent* materialContent = new MaterialContent();
			materialContent->SetContentIdentity(m_identity);
			
			aiString name;
			aiMaterial->Get(AI_MATKEY_NAME, name);
			materialContent->SetName(name.C_Str());

			GetTexturesForMaterial(materialContent, directory, aiMaterial);
			
			m_materials.push_back(materialContent);
		}
	}

	Matrix ModelImporter::GetRelativeTransform(const aiNode* node, const aiNode* ancestor)
	{
		aiMatrix4x4 aiTransform = node->mTransformation;
		aiNode* parent = node->mParent;
		while (parent != nullptr && parent != ancestor)
		{
			aiTransform *= parent->mTransformation;
			parent = parent->mParent;
		}

		return ToMatrix(aiTransform);
	}

	Matrix ModelImporter::ToMatrix(const aiMatrix4x4& aiMatrix)
	{
		Matrix transform;
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

	NodeContent* ModelImporter::ImportNode(const aiScene* scene, const aiNode* currentNode, const aiNode* parentNode, NodeContent* parentContent)
	{
		NodeContent* nodeContent = nullptr;
		Matrix nodeTransform = ToMatrix(currentNode->mTransformation);

		if (currentNode->mNumMeshes > 0)
		{
			MeshContent* meshContent = new MeshContent();
			meshContent->SetName(currentNode->mName.C_Str());
			meshContent->SetContentIdentity(m_identity);
			meshContent->GetTransform() = GetRelativeTransform(currentNode, parentNode);

			for (uint32_t i = 0; i < currentNode->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[currentNode->mMeshes[i]];

				if (mesh->mNumVertices == 0)
					continue;

				GeometryContent* geometryContent = CreateGeometry(mesh, meshContent);
				meshContent->SetName(currentNode->mName.C_Str());

				meshContent->GetGeometry().push_back(geometryContent);
			}

			nodeContent = meshContent;
		}
		else
		{
			nodeContent = new NodeContent();
			nodeContent->SetName(currentNode->mName.C_Str());
			nodeContent->SetContentIdentity(m_identity);
			nodeContent->GetTransform() = GetRelativeTransform(currentNode, parentNode);
		}
		//nodeContent->GetTransform() = nodeTransform;
		
		if (nodeContent)
		{
			nodeContent->GetParent() = parentContent;
			if (parentContent)
			{
				parentContent->GetChildren().push_back(nodeContent);
			}
		}

		for (uint32_t i = 0; i < currentNode->mNumChildren; i++)
		{
			aiNode* childNode = currentNode->mChildren[i];
			ImportNode(scene, childNode, currentNode, nodeContent);
		}

		return nodeContent;
	}
}