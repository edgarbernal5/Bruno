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
	class ModelMeshContent;

	class BRUNO_API_EXPORT ModelProcessor : public IContentProcessor
	{
	public:
		ModelProcessor();
		~ModelProcessor();

		ContentItem* Process(ContentItem* input, ContentProcessorContext *context);

		friend class BuildCoordinator;

		std::string GetProcessorName() const { return "ModelProcessor"; }
		std::string GetOutputWriterName() const { return "ModelWriter"; }

	private:
		MaterialContent* ConvertMaterial(MaterialContent* material, ContentProcessorContext* context);
		void ProcessGeometryUsingMaterial(MaterialContent* material, std::vector< GeometryContent*> & geometryList, ContentProcessorContext* context);
		void GetFlattenedNodes(NodeContent* input, std::vector<NodeContent*> &outputNodes);

		ModelBoneContent* ProcessNode(NodeContent* node, ModelBoneContent* parent, std::vector< ModelBoneContent*>& boneList, std::vector< ModelMeshContent*>& meshList, ContentProcessorContext* context);
		ModelMeshContent* ProcessMesh(MeshContent* mesh, ModelBoneContent* parent, ContentProcessorContext* context);
	};
}