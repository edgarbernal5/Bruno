#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <map>

namespace BrunoEngine
{
	class Texture;

	class BRUNO_API_EXPORT Material
	{
	public:
		Material();
		Material(std::string& name);
		~Material();

		inline const Vector3& GetDiffuseColor()
		{
			return m_diffuseColor;
		}

		inline const char* GetName()
		{
			return m_name.c_str();
		}

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
		
		inline void SetDiffuseColor(Vector3& color)
		{
			m_diffuseColor = color;
		}

		inline void SetName(std::string& name)
		{
			m_name = name;
		}

	private:
		std::string m_name;

		Vector3 m_diffuseColor;

		std::map<std::string, Texture*> m_textures;
	};
}