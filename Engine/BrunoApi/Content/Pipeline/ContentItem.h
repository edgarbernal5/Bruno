#pragma once

#include "TrioApiRequisites.h"
#include "Content/Pipeline/OpaqueData.h"
#include "ContentIdentity.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT ContentItem
	{
	public:
		ContentItem();
		virtual ~ContentItem() {}

		//inline int GetID()
		//{
		//	return m_ID;
		//}

		void SetName(std::string name)
		{
			m_name = name;
		}

		inline std::string GetName()
		{
			return m_name;
		}

		void SetContentIdentity(ContentIdentity identity)
		{
			m_contentIdentity = identity;
		}

		inline ContentIdentity GetContentIdentity()
		{
			return m_contentIdentity;
		}

		inline OpaqueData& GetOpaqueData()
		{
			return m_opaqueData;
		}

		friend class ContentWriter;
		friend class ContentCompiler;
		friend class GameContentBuilder;

	private:
		//int m_ID;
		OpaqueData m_opaqueData;

		std::string m_name;

	protected:
		std::string m_importerName;
		std::string m_processorName;

		ContentIdentity m_contentIdentity;
	};
}