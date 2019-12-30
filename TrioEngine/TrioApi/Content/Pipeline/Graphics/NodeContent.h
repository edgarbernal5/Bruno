#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT NodeContent : public ContentItem
	{
	public:
		NodeContent();
		virtual ~NodeContent() {}

		inline NodeContent* GetParent() {
			return m_parent;
		}

		inline Matrix& GetTransform()
		{
			return m_transform;
		}
		inline std::vector<NodeContent*>& GetChildren()
		{
			return m_children;
		}
	private:
		NodeContent* m_parent;
		Matrix m_transform;

		std::vector<NodeContent*> m_children;
	};
}