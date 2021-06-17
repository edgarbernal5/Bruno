#include "stdafx.h"
#include "MaterialReader.h"

#include "Content/GraphicsContentHelper.h"
#include "Content/ContentReader.h"

#include "Graphics/Models/Material.h"

namespace BrunoEngine
{
	MaterialReader::MaterialReader()
	{
	}


	MaterialReader::~MaterialReader()
	{
	}

	void* MaterialReader::Read(ContentReader* input)
	{
		Material* material = new Material();

		std::string name = input->ReadString();
		material->SetName(name);

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