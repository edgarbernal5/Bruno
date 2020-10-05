#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <map>

namespace TrioEngine
{
	class Texture;

	class TRIO_API_EXPORT Material
	{
	public:
		Material();
		~Material();

		inline std::map<std::string, Texture*>& GetTextures()
		{
			return m_textures;
		}

		Texture* GetTextureByName(std::string name)
		{
			auto itm = m_textures.find(name);
			if (itm == m_textures.end())
				return nullptr;

			return itm->second;
		}
		inline void InsertTexture(std::string name, Texture* texture)
		{
			m_textures[name] = texture;
		}

	private:
		std::map<std::string, Texture*> m_textures;
	};
}