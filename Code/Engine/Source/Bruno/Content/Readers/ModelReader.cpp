#include "brpch.h"
#include "ModelReader.h"

#include "Bruno/Content/ContentReader.h"

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

		for (size_t i = 0; i < materialCount; i++)
		{
			std::string name;
			input.ReadString(name);

			size_t texturexCount;
			input.ReadUInt64(texturexCount);
			for (size_t j = 0; j < texturexCount; j++)
			{
				std::string textureName;
				input.ReadString(textureName);

				auto texture = input.ReadExternalReference();
			}
			
		}

		size_t meshesCount;
		input.ReadUInt64(meshesCount);

		std::vector<Mesh> meshes;
		meshes.reserve(meshesCount);

		for (size_t i = 0; i < meshesCount; i++)
		{
			std::string name;
			input.ReadString(name);
			
			uint32_t materialIndex;
			input.ReadUInt32(materialIndex);

			size_t count;
			input.ReadUInt64(count);
			std::vector<Math::Vector3> vertices;

			for (size_t j = 0; j < count; j++)
			{
				Math::Vector3 vertex;
				input.ReadVector3(vertex);

				vertices.push_back(vertex);
			}
		}
		auto model = std::make_shared<Model>();
		return model;
	}
}