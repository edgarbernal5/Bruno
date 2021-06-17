#include "stdafx.h"
#include "ModelWriter.h"

#include "Content/Pipeline/Processors/ModelBoneContent.h"
#include "Content/Pipeline/Processors/ModelMeshContent.h"
#include "Content/Pipeline/Processors/ModelContent.h"
#include "Content/Pipeline/Processors/ModelMeshPartContent.h"
#include "Content/Pipeline/Graphics/VertexBufferContent.h"
#include "Content/Pipeline/Graphics/IndexCollection.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"

#include "Content/Pipeline/Serialization/ContentWriter.h"

namespace TrioEngine
{
	ModelWriter::ModelWriter()
	{
	}

	ModelWriter::~ModelWriter()
	{
	}

	void ModelWriter::Write(ContentWriter* output, ContentItem* value)
	{
		ModelContent* content = (ModelContent*)value;

		WriteBones(output, content);
		WriteMeshes(output, content);
		WriteBoneIndex(output, content->m_bones, content->m_root);
	}

	void ModelWriter::WriteBones(ContentWriter* output, ModelContent* content)
	{
		auto bones = content->m_bones;

		output->WriteUInt32(bones.size());
		for (int i = 0; i < bones.size(); i++)
		{
			output->WriteString(bones[i]->GetName());
			output->Write(bones[i]->GetTransform());
		}
		for (int i = 0; i < bones.size(); i++)
		{
			WriteBoneIndex(output, content->m_bones, bones[i]->GetParent());

			auto children = bones[i]->GetChildren();
			output->WriteUInt32(children.size());

			for (int j = 0; j < children.size(); j++)
			{
				WriteBoneIndex(output, content->m_bones, children[j]);
			}
		}
	}

	void ModelWriter::WriteBoneIndex(ContentWriter* output, std::vector<ModelBoneContent*>& bones, ModelBoneContent* bone)
	{
		int totalBones = bones.size() + 1;
		int boneId = (bone == nullptr) ? 0 : (bone->GetIndex() + 1);
		if (totalBones <= 0xff)
		{
			output->WriteByte((uint8_t)boneId);
			return;
		}

		output->WriteInt32(boneId);
	}

	void ModelWriter::WriteMeshes(ContentWriter* output, ModelContent* content)
	{
		auto &meshes = content->m_meshes;

		output->WriteUInt32(meshes.size());
		for (int i = 0; i < meshes.size(); i++)
		{
			output->WriteString(meshes[i]->GetName());
			WriteBoneIndex(output, content->m_bones, meshes[i]->GetParentBone());

			/*output->Write(m_meshes[i]->GetBoundingSphere().Center);
			output->WriteSingle(m_meshes[i]->GetBoundingSphere().Radius);*/

			//output->Write(m_meshes[i]->GetBoundingBox().Min);
			//output->Write(m_meshes[i]->GetBoundingBox().Max);

			WriteMeshParts(output, meshes[i]->GetMeshParts());
		}
	}

	void ModelWriter::WriteMeshParts(ContentWriter* output, const std::vector<ModelMeshPartContent*>& meshParts)
	{
		output->WriteUInt32(meshParts.size());
		for (int i = 0; i < meshParts.size(); i++)
		{
			ModelMeshPartContent* part = meshParts[i];

			output->WriteInt32(part->GetVertexOffset());
			output->WriteInt32(part->GetNumVertices());
			output->WriteInt32(part->GetStartIndex());
			output->WriteInt32(part->GetPrimitiveCount());

			output->WriteSharedResource(part->GetVertexBuffer());
			output->WriteSharedResource(part->GetIndexBuffer());
			output->WriteSharedResource(part->GetMaterial());
		}
	}
}