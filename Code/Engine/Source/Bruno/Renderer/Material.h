#pragma once

#include "Bruno/Core/RTTI.h"
#include <string>
#include <map>


namespace Bruno
{
	class Texture;

	class Material : public RTTI
	{
		BR_RTTI_DECLARATION(Material, RTTI);

	public:

		std::string Name;
		std::map<std::string, std::shared_ptr<Texture>> TexturesByName;
	};
}
