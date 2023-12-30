#include "brpch.h"
#include "ModelReader.h"

#include "Bruno/Content/ContentReader.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"
#include "Bruno/Platform/DirectX/VertexTypes.h"

#include "Bruno/Renderer/Material.h"
#include "Bruno/Platform/DirectX/Texture.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelReader);

	ModelReader::ModelReader() :
		ContentTypeReader(Model::GetTypeIdClass())
	{
	}

	std::shared_ptr<Model> ModelReader::ReadInternal(ContentReader& input)
	{
		size_t materialCount;
		input.ReadUInt64(materialCount);

		std::vector<std::shared_ptr<Material>> materials;
		for (size_t i = 0; i < materialCount; i++)
		{
			auto material = std::make_shared<Material>();
			input.ReadString(material->Name);

			size_t texturexCount;
			input.ReadUInt64(texturexCount);
			for (size_t j = 0; j < texturexCount; j++)
			{
				std::string textureName;
				input.ReadString(textureName);

				auto texture = input.ReadExternalReference();
				material->TexturesByName[textureName] = std::static_pointer_cast<Texture>(texture);
			}

			materials.emplace_back(std::move(material));
		}

		size_t verticesCount;
		input.ReadUInt64(verticesCount);
		std::vector<ModelVertex> vertices;
		vertices.reserve(verticesCount);

		for (size_t j = 0; j < verticesCount; j++)
		{
			auto& vertex = vertices.emplace_back();
			input.ReadVector3(vertex.Position);
			input.ReadVector3(vertex.Normal);
			input.ReadVector3(vertex.Tangent);
			input.ReadVector3(vertex.Binormal);
			input.ReadVector3(vertex.Texcoord);
		}

		size_t meshesCount;
		input.ReadUInt64(meshesCount);

		std::vector<std::shared_ptr<Mesh>> meshes;
		meshes.reserve(meshesCount);

		for (size_t i = 0; i < meshesCount; i++)
		{
			std::string meshName;
			input.ReadString(meshName);

			std::string nodeName;
			input.ReadString(nodeName);

			uint32_t baseVertex;
			input.ReadUInt32(baseVertex);

			uint32_t baseIndex;
			input.ReadUInt32(baseIndex);

			uint32_t vertexCount;
			input.ReadUInt32(vertexCount);

			uint32_t indexCount;
			input.ReadUInt32(indexCount);

			uint32_t materialIndex;
			input.ReadUInt32(materialIndex);

			Math::Matrix transform;
			input.ReadMatrix(transform);

			Math::Matrix localTransform;
			input.ReadMatrix(localTransform);

			Math::BoundingBox bbox;
			input.ReadBoundingBox(bbox);			

			auto mesh = std::make_shared<Mesh>(meshName, baseVertex, baseIndex, vertexCount, indexCount, materialIndex, transform, localTransform, bbox);
			meshes.emplace_back(std::move(mesh));
		}

		size_t indicesCount;
		input.ReadUInt64(indicesCount);
		std::vector<uint32_t> indices;
		indices.reserve(indicesCount);

		for (size_t i = 0; i < indicesCount; i++)
		{
			uint32_t index;
			input.ReadUInt32(index);

			indices.push_back(index);
		}

		size_t modelNodesCount;
		input.ReadUInt64(modelNodesCount);
		std::vector<ModelNode> modelNodes;
		modelNodes.reserve(modelNodesCount);
		for (size_t i = 0; i < modelNodesCount; i++)
		{
			auto& modelNode = modelNodes.emplace_back();
			input.ReadUInt32(modelNode.Parent);

			size_t childrenCount;
			input.ReadUInt64(childrenCount);

			modelNode.Children.reserve(childrenCount);
			for (size_t j = 0; j < childrenCount; j++)
			{
				uint32_t child;
				input.ReadUInt32(child);
				modelNode.Children.push_back(child);
			}
			size_t meshesCount;
			input.ReadUInt64(meshesCount);

			modelNode.Meshes.reserve(meshesCount);
			for (size_t j = 0; j < meshesCount; j++)
			{
				uint32_t meshIndex;
				input.ReadUInt32(meshIndex);
				modelNode.Meshes.push_back(meshIndex);
			}

			input.ReadString(modelNode.Name);
			input.ReadMatrix(modelNode.LocalTransform);
		}

		std::vector<VertexPositionNormalTexture> verticesPNT;
		verticesPNT.reserve(vertices.size());
		for (size_t j = 0; j < vertices.size(); j++)
		{
			auto& vertex = verticesPNT.emplace_back();
			vertex.Position = vertices[j].Position;
			vertex.Normal = vertices[j].Normal;
			auto& texCoord3D = vertices[j].Texcoord;
			vertex.Texture.x = texCoord3D.x;
			vertex.Texture.y = texCoord3D.y;
		}

		auto indexBuffer = std::make_shared<IndexBuffer>(static_cast<uint32_t>(indicesCount * sizeof(uint32_t)), indices.data(), sizeof(uint32_t));
		auto vertexBuffer = std::make_shared<VertexBuffer>(static_cast<uint32_t>(vertices.size() * sizeof(VertexPositionNormalTexture)), verticesPNT.data(), sizeof(VertexPositionNormalTexture));
				
		auto model = std::make_shared<Model>(std::move(vertices), std::move(indices), std::move(materials), std::move(meshes), std::move(modelNodes));
		model->SetIndexBuffer(indexBuffer);
		model->SetVertexBuffer(vertexBuffer);

		return model;
	}
}