#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"
#include "Content/Pipeline/ExternalReference.h"

namespace TrioEngine
{
	class BRUNO_API_EXPORT MaterialContent : public ContentItem
	{
	public:
		MaterialContent();
		virtual ~MaterialContent() {}

		inline std::map<std::string, ExternalReference>& GetTextures()
		{
			return m_textures;
		}

		bool operator <(const MaterialContent &other) const
		{
			return m_textures.size() < other.m_textures.size();
		}
		bool operator <(const MaterialContent* &other) const
		{
			return m_textures.size() < other->m_textures.size();
		}

		// Obtiene la textura difusa.
		inline ExternalReference& GetTexture()
		{
			return GetTexture("Texture");
		}

		inline void SetTexture(std::string key, ExternalReference texture)
		{
			if (texture.GetFilename().size() == 0)
			{
				std::map<std::string, ExternalReference>::iterator itm = m_textures.find(key);
				if (itm != m_textures.end())
					m_textures.erase(itm);
			}
			else
			{
				m_textures[key] = texture;
			}
		}
	private:
		std::map<std::string, ExternalReference> m_textures;

	protected:
		ExternalReference& GetTexture(std::string key);
	};
}