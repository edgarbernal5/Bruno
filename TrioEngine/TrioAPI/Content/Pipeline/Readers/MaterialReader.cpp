#include "stdafx.h"
#include "MaterialReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/Models/Material.h"

namespace TrioEngine
{
	MaterialReader::MaterialReader()
	{
	}


	MaterialReader::~MaterialReader()
	{
	}

	void* MaterialReader::Read(ContentReader* input)
	{
		/*MaterialContent* inputContent = reinterpret_cast<MaterialContent*>(value);

		output->WriteUInt32(inputContent->GetTextures().size());
		for (auto& pair : inputContent->GetTextures()) {
			output->WriteString(pair.first);
			output->WriteExternalReference(pair.second);
		}*/

		Material* material = new Material();

		uint32_t textureCount = input->ReadUInt32();
		for (size_t i = 0; i < textureCount; i++)
		{
			std::string name = input->ReadString();

			Texture* texture = reinterpret_cast<Texture*>(input->ReadExternalReference());
			material->InsertTexture(name, texture);
		}
		return material;
	}
}