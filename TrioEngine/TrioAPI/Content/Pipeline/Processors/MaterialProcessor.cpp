#include "stdafx.h"
#include "MaterialProcessor.h"

//#include "Content/Pipeline/Graphics/EffectMaterialContent.h"
#include "Content/Pipeline/Processors/ContentProcessorContext.h"
#include "Content/Pipeline/Graphics/MaterialContent.h"

namespace TrioEngine
{
	MaterialProcessor::MaterialProcessor()
	{
	}


	MaterialProcessor::~MaterialProcessor()
	{
	}

	void MaterialProcessor::BuildAllTextures(MaterialContent* input, ContentProcessorContext* context)
	{
		//BasicMaterialContent* content = reinterpret_cast<BasicMaterialContent*>(input);

		//if (content != nullptr && content->GetTexture().GetFilename().size() > 0)
		//{
		//	content->SetTexture("Texture", BuildTexture("Texture", content->GetTexture(), context));
		//}
		//else
		{
			std::vector<std::pair<std::string, ExternalReference>> list;
			std::map<std::string, ExternalReference>::iterator itm;

			for (itm = input->GetTextures().begin(); itm != input->GetTextures().end(); itm++)
			{
				auto item = make_pair(itm->first, BuildTexture(itm->first, itm->second, context));
				list.push_back(item);
			}

			for (int i = 0; i < list.size(); i++)
			{
				input->SetTexture(list[i].first, list[i].second);
			}
		}
	}

	ExternalReference MaterialProcessor::BuildTexture(std::string textureName, ExternalReference texture, ContentProcessorContext* context)
	{
		return context->BuildAsset(texture, "TextureProcessor", nullptr, "", "");
	}

	ExternalReference MaterialProcessor::BuildEffect(ExternalReference effect, ContentProcessorContext* context)
	{
		return context->BuildAsset(effect, "EffectProcessor", nullptr, "", "");
	}

	ContentItem* MaterialProcessor::Process(ContentItem* input, ContentProcessorContext* context)
	{
		MaterialContent* materialInput = (MaterialContent*)input;
		BuildAllTextures(materialInput, context);

		//EffectMaterialContent* effectMaterialContent = dynamic_cast<EffectMaterialContent*>(materialInput);

		//if (effectMaterialContent != nullptr)
		//{
		//	if (effectMaterialContent->GetEffect().GetFilename().size() == 0)
		//	{
		//		throw std::exception("");
		//	}
		//	effectMaterialContent->GetCompiledEffect() = BuildEffect(effectMaterialContent->GetEffect(), context);
		//}
		return input;
	}
}