#pragma once

#include "TrioApiRequisites.h"

#include "IContentProcessor.h"
#include <vector>

namespace BrunoEngine
{
	class NodeContent;
	class MaterialContent;
	class GeometryContent;
	class ModelBoneContent;
	class MeshContent;
	class SceneMeshContent;

	class BRUNO_API_EXPORT SceneModelProcessor : public IContentProcessor
	{
	public:
		SceneModelProcessor();
		~SceneModelProcessor();

		ContentItem* Process(ContentItem* input, ContentProcessorContext *context);

		friend class GameContentBuilder;

		std::string GetProcessorName() const { return "SceneModelProcessor"; }
		std::string GetOutputWriterName() const { return "SceneModelWriter"; }

	private:
		MaterialContent* ConvertMaterial(MaterialContent* material, ContentProcessorContext* context);
		void ProcessGeometryUsingMaterial(MaterialContent* material, std::vector< GeometryContent*> & geometryList, ContentProcessorContext* context);
		void GetFlattenedNodes(NodeContent* input, std::vector<NodeContent*> &outputNodes);

		ModelBoneContent* ProcessNode(NodeContent* node, ModelBoneContent* parent, std::vector< ModelBoneContent*>& boneList, std::vector< SceneMeshContent*>& meshList, ContentProcessorContext* context);
		SceneMeshContent* ProcessMesh(MeshContent* mesh, ModelBoneContent* parent, ContentProcessorContext* context);
	};
}