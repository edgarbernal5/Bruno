#include "stdafx.h"
#include "SceneWriter.h"

#include "Content/Pipeline/Processors/ModelBoneContent.h"
#include "Content/Pipeline/Processors/SceneMeshContent.h"
#include "Content/Pipeline/Processors/SceneContent.h"
#include "Content/Pipeline/Processors/SceneSubMeshContent.h"
#include "Content/Pipeline/Graphics/VertexBufferContent.h"
#include "Content/Pipeline/Graphics/IndexCollection.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"

#include "Content/Pipeline/Serialization/ContentWriter.h"

namespace BrunoEngine
{
	SceneWriter::SceneWriter()
	{
	}

	SceneWriter::~SceneWriter()
	{
	}

	void SceneWriter::Write(ContentWriter* output, ContentItem* value)
	{
		SceneContent* content = (SceneContent*)value;

		WriteBones(output, content);
		WriteMaterials(output, content);
		WriteMeshes(output, content);
		WriteBoneIndex(output, content->m_bones, content->m_root);
	}

	void SceneWriter::WriteBones(ContentWriter* output, SceneContent* content)
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

	void SceneWriter::WriteMaterials(ContentWriter* output, SceneContent* content)
	{
		auto materials = content->GetMaterials();
		output->WriteUInt32(materials.size());
		for (size_t i = 0; i < materials.size(); i++)
		{
			output->WriteSharedResource(materials[i]);
		}
	}

	void SceneWriter::WriteBoneIndex(ContentWriter* output, std::vector<ModelBoneContent*>& bones, ModelBoneContent* bone)
	{
		int totalBones = bones.size() + 1;
		int boneId = (bone == nullptr) ? 0 : (bone->GetIndex() + 1);
		if (totalBones <= 0xff)
		{
			output->WriteByte((uint8_t)boneId);
		}
		else
		{
			output->WriteInt32(boneId);
		}
	}

	void SceneWriter::WriteMeshes(ContentWriter* output, SceneContent* content)
	{
		auto meshes = content->m_meshes;

		output->WriteUInt32(meshes.size());
		for (int i = 0; i < meshes.size(); i++)
		{
			output->WriteString(meshes[i]->GetName());
			WriteBoneIndex(output, content->m_bones, meshes[i]->GetParentBone());

			/*output->Write(m_meshes[i]->GetBoundingSphere().Center);
			output->WriteSingle(m_meshes[i]->GetBoundingSphere().Radius);*/

			//output->Write(m_meshes[i]->GetBoundingBox().Min);
			//output->Write(m_meshes[i]->GetBoundingBox().Max);

			WriteSubMeshes(output, meshes[i]->GetSubMeshes());
		}
	}

	void SceneWriter::WriteSubMeshes(ContentWriter* output, const std::vector<SceneSubMeshContent*>& subMeshes)
	{
		output->WriteUInt32(subMeshes.size());
		for (int i = 0; i < subMeshes.size(); i++)
		{
			SceneSubMeshContent* subMesh = subMeshes[i];

			output->WriteInt32(subMesh->GetVertexOffset());
			output->WriteInt32(subMesh->GetNumVertices());
			output->WriteInt32(subMesh->GetStartIndex());
			output->WriteInt32(subMesh->GetPrimitiveCount());

			output->WriteSharedResource(subMesh->GetVertexBuffer());
			output->WriteSharedResource(subMesh->GetIndexBuffer());
			output->WriteSharedResource(subMesh->GetMaterial());
		}
	}
}