#include "ModelWriter.h"

#include "Pipeline/Graphics/ModelContentItem.h"
#include "Pipeline/Graphics/MaterialContentItem.h"

namespace Bruno
{
	ModelWriter::ModelWriter() :
		AbstractContentTypeWriter(ModelContentItem::GetTypeIdClass())
	{
	}

	std::string ModelWriter::GetReaderName()
	{
		return "ModelReader";
	}

	void ModelWriter::Write(ContentWriter& output, const ContentItem& contentItem)
	{
		auto modelContentItem = contentItem.As<ModelContentItem>();
		
		output.WriteUInt64(modelContentItem->Materials.size());
		for (size_t i = 0; i < modelContentItem->Materials.size(); i++)
		{
			output.WriteString(modelContentItem->Materials[i]->Name);
			output.WriteUInt64(modelContentItem->Materials[i]->TexturesByKeys.size());
			for (auto& [name, reference] : modelContentItem->Materials[i]->TexturesByKeys)
			{
				output.WriteString(name);
				output.WriteExternalReference(reference);
			}
		}

		//Vertices.
		output.WriteUInt64(modelContentItem->Vertices.size());
		for (size_t i = 0; i < modelContentItem->Vertices.size(); i++)
		{
			output.WriteVector3(modelContentItem->Vertices[i].Position);
			output.WriteVector3(modelContentItem->Vertices[i].Normal);
			output.WriteVector3(modelContentItem->Vertices[i].Tangent);
			output.WriteVector3(modelContentItem->Vertices[i].Binormal);
			output.WriteVector3(modelContentItem->Vertices[i].Texcoord);
		}

		output.WriteUInt64(modelContentItem->Meshes.size());
		for (size_t i = 0; i < modelContentItem->Meshes.size(); i++)
		{
			output.WriteString(modelContentItem->Meshes[i]->MeshName);
			output.WriteString(modelContentItem->Meshes[i]->NodeName);
			output.WriteUInt32(modelContentItem->Meshes[i]->BaseVertex);
			output.WriteUInt32(modelContentItem->Meshes[i]->BaseIndex);
			output.WriteUInt32(modelContentItem->Meshes[i]->VertexCount);
			output.WriteUInt32(modelContentItem->Meshes[i]->IndexCount);
			output.WriteUInt32(modelContentItem->Meshes[i]->MaterialIndex);
			output.WriteMatrix(modelContentItem->Meshes[i]->Transform);
			output.WriteMatrix(modelContentItem->Meshes[i]->LocalTransform);
			output.WriteBoundingBox(modelContentItem->Meshes[i]->BBox);
			
		}
		//Indices.
		output.WriteUInt64(modelContentItem->Indices.size());
		for (size_t j = 0; j < modelContentItem->Indices.size(); j++)
		{
			output.WriteUInt32(modelContentItem->Indices[j]);
		}
	}
}
