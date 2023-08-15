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

		output.WriteUInt64(modelContentItem->Meshes.size());
		for (size_t i = 0; i < modelContentItem->Meshes.size(); i++)
		{
			output.WriteString(modelContentItem->Meshes[i]->Name);
			output.WriteUInt32(modelContentItem->Meshes[i]->MaterialIndex);
			
			//vertices.
			output.WriteUInt64(modelContentItem->Meshes[i]->Vertices.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->Vertices.size(); j++)
			{
				output.WriteVector3(modelContentItem->Meshes[i]->Vertices[j]);
			}

			//normals.
			output.WriteUInt64(modelContentItem->Meshes[i]->Normals.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->Normals.size(); j++)
			{
				output.WriteVector3(modelContentItem->Meshes[i]->Normals[j]);
			}
			
			//tangents.
			output.WriteUInt64(modelContentItem->Meshes[i]->Tangents.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->Tangents.size(); j++)
			{
				output.WriteVector3(modelContentItem->Meshes[i]->Tangents[j]);
			}
			
			//binormals.
			output.WriteUInt64(modelContentItem->Meshes[i]->BiNormals.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->BiNormals.size(); j++)
			{
				output.WriteVector3(modelContentItem->Meshes[i]->BiNormals[j]);
			}
			
			//texture coordinates.
			output.WriteUInt64(modelContentItem->Meshes[i]->TextureCoordinates.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->TextureCoordinates.size(); j++)
			{
				output.WriteUInt64(modelContentItem->Meshes[i]->TextureCoordinates[j].size());
				for (size_t k = 0; k < modelContentItem->Meshes[i]->TextureCoordinates[j].size(); k++)
				{
					output.WriteVector3(modelContentItem->Meshes[i]->TextureCoordinates[j][k]);
				}
			}
			
			output.WriteUInt32(modelContentItem->Meshes[i]->FaceCount);
			output.WriteUInt64(modelContentItem->Meshes[i]->Indices.size());
			for (size_t j = 0; j < modelContentItem->Meshes[i]->Indices.size(); j++)
			{
				output.WriteUInt32(modelContentItem->Meshes[i]->Indices[j]);
			}
		}
	}
}
