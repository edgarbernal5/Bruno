#pragma once

#include "TrioApiRequisites.h"

#include <vector>
#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT IndexCollection : public ContentItem
	{
	public:

		typedef std::vector<int>::iterator       iterator;
		typedef std::vector<int>::const_iterator const_iterator;

		IndexCollection()
		{
			m_processorName = "IndexCollectionWriter";
		}

		~IndexCollection()
		{
		}

		void AddRange(const_iterator begin, const_iterator end)
		{
			m_indices.insert(m_indices.end(), begin, end);
		}

		const int & operator[](int nIndex) const
		{
			return m_indices[nIndex];
		}

		int & operator[](int nIndex)
		{
			return m_indices[nIndex];
		}

		void push_back(int item)
		{
			m_indices.push_back(item);
		}

		size_t size() const
		{
			return m_indices.size();
		}

		void clear()
		{
			m_indices.clear();
		}

		iterator       begin() { return m_indices.begin(); }
		iterator       end() { return m_indices.end(); }
		const_iterator begin() const { return m_indices.begin(); }
		const_iterator end()   const { return m_indices.end(); }
	private:
		std::vector<int> m_indices;
	};
}