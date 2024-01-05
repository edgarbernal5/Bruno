#pragma once

#include "Bruno/Content/Asset.h"
#include <string>
#include <map>


namespace Bruno
{
	class Texture;

	class Material : public Asset
	{
		BR_RTTI_DECLARATION(Material, Asset);

	public:
		std::string Name;
		std::map<std::string, std::shared_ptr<Texture>> TexturesByName;
	};
}
