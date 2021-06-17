#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class BRUNO_API_EXPORT NodeContent : public ContentItem
	{
	public:
		NodeContent();
		virtual ~NodeContent() {}

		inline const NodeContent* GetParent() const {
			return m_parent;
		}

		inline NodeContent*& GetParent() {
			return m_parent;
		}

		inline Matrix& GetTransform()
		{
			return m_transform;
		}

		inline Matrix GetAbsoluteTransform()
		{
			if (m_parent) {
				return m_transform * m_parent->GetAbsoluteTransform();
			}
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