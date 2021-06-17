#include "stdafx.h"
#include "SceneModelProcessor.h"

#include "Content/Pipeline/Graphics/NodeContent.h"
#include "Content/Pipeline/Graphics/MeshContent.h"
#include "Content/Pipeline/Graphics/GeometryContent.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"
#include "Content/Pipeline/Graphics/VertexBufferContent.h"
#include "Content/Pipeline/Graphics/IndexCollection.h"
#include "Content/Pipeline/Processors/SceneContent.h"
#include "Content/Pipeline/Processors/ModelBoneContent.h"
#include "Content/Pipeline/Processors/SceneMeshContent.h"
#include "Content/Pipeline/Processors/SceneSubMeshContent.h"

#include "Content/Pipeline/Graphics/VertexContent.h"
#include "Content/Pipeline/Graphics/VertexChannelCollection.h"

#include "Content/Pipeline/Processors/ContentProcessorContext.h"

namespace TrioEngine
{
	SceneModelProcessor::SceneModelProcessor()
	{
	}

	SceneModelProcessor::~SceneModelProcessor()
	{
	}

	ContentItem* SceneModelProcessor::Process(ContentItem* input, ContentProcessorContext *context)
	{
		NodeContent* inputContent = (NodeContent*)input;

		std::vector<NodeContent*> nodes;
		GetFlattenedNodes(inputContent, nodes);
		std::vector<GeometryContent*> geometries;
		std::vector<MaterialContent*> materials;

		for (auto& node : nodes) {
			auto mesh = dynamic_cast<MeshContent*>(node);
			if (mesh) {
				for (auto& geometry : mesh->GetGeometry()) {
					geometries.push_back(geometry);

					materials.push_back(geometry->GetMaterial());
				}
			}
		}

		std::vector<MaterialContent*> processedMaterials;
		for (auto& material : materials) {
			std::vector< GeometryContent*> outcomeGeoms;
			for (auto& geometry : geometries) {
				if (geometry->GetMaterial() == material) {
					outcomeGeoms.push_back(geometry);
				}
			}

			MaterialContent* processedMaterial = ConvertMaterial(material, context);
			ProcessGeometryUsingMaterial(processedMaterial, outcomeGeoms, context);

			processedMaterials.push_back(processedMaterial);
		}

		std::vector< ModelBoneContent*> boneList;
		std::vector< SceneMeshContent*> meshList;
		ModelBoneContent* rootNode = ProcessNode(inputContent, nullptr, boneList, meshList, context);
		
		return new SceneContent(rootNode, boneList, processedMaterials, meshList);
	}

	MaterialContent* SceneModelProcessor::ConvertMaterial(MaterialContent* material, ContentProcessorContext* context)
	{
		return (MaterialContent*)context->Convert(material, "MaterialProcessor", nullptr);
	}

	void SceneModelProcessor::ProcessGeometryUsingMaterial(MaterialContent* material, std::vector< GeometryContent*>& geometryList, ContentProcessorContext* context)
	{
		//Si no tiene material, fallbackear a uno.

		for (auto& geometry : geometryList) {
			geometry->SetMaterial(material);
		}
	}

	void SceneModelProcessor::GetFlattenedNodes(NodeContent* input, std::vector<NodeContent*>& outputNodes)
	{
		outputNodes.push_back(input);
		for (auto& child : input->GetChildren()) {
			GetFlattenedNodes(child, outputNodes);
		}
	}

	ModelBoneContent* SceneModelProcessor::ProcessNode(NodeContent* node, ModelBoneContent* parent, std::vector< ModelBoneContent*>& boneList, std::vector< SceneMeshContent*>& meshList, ContentProcessorContext* context)
	{
		ModelBoneContent* boneContent = new ModelBoneContent(node->GetName(), boneList.size(), node->GetTransform(), parent);
		boneList.push_back(boneContent);

		auto nodeAsMesh = dynamic_cast<MeshContent*>(node);
		if (nodeAsMesh) {
			meshList.push_back(ProcessMesh(nodeAsMesh, boneContent, context));
		}

		std::vector< ModelBoneContent*> children;
		for (auto& child : node->GetChildren()) {
			children.push_back(ProcessNode(child, boneContent, boneList, meshList, context));
		}

		boneContent->SetChildren(children);

		return boneContent;
	}

	SceneMeshContent* SceneModelProcessor::ProcessMesh(MeshContent* mesh, ModelBoneContent* parent, ContentProcessorContext* context)
	{
		VertexBufferContent* vertexBuffer = new VertexBufferContent();
		IndexCollection* indexBuffer = new IndexCollection();
		std::vector< SceneSubMeshContent*> parts;

		int startVertex = 0;
		for (auto& geometry : mesh->GetGeometry()) {
			auto vertices = geometry->GetVertices();
			auto vertexCount = vertices->GetVertexCount();

			SceneSubMeshContent* subMeshContent;
			if (vertexCount == 0) {
				subMeshContent = new SceneSubMeshContent();
			}
			else {
				auto geometryBuffer = vertices->CreateVertexBuffer();
				vertexBuffer->Write(vertexBuffer->GetVertexDataSize(), 1, geometryBuffer->GetVertexData(), geometryBuffer->GetVertexDataSize());

				int startIndex = indexBuffer->size();
				indexBuffer->AddRange(geometry->GetIndices().cbegin(), geometry->GetIndices().cend());

				subMeshContent = new SceneSubMeshContent(vertexBuffer, indexBuffer, startVertex, vertexCount, startIndex, geometry->GetIndices().size() / 3);

				vertexBuffer->SetVertexDeclaration(geometryBuffer->GetVertexDeclaration());

				startVertex += vertexCount;
			}

			subMeshContent->SetMaterial(geometry->GetMaterial());
			parts.push_back(subMeshContent);
		}

		return new SceneMeshContent(mesh->GetName(), mesh, parent, parts);
	}
}