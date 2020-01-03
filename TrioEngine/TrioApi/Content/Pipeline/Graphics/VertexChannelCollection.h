#pragma once

#include "TrioApiRequisites.h"

#include "VertexChannel.h"
namespace TrioEngine
{
	class VertexChannel;
	class VertexContent;

	class TRIO_API_EXPORT VertexChannelCollection
	{
	public:
		typedef VertexChannel* BaseType;

		VertexChannelCollection(VertexContent* parent);
		~VertexChannelCollection();

		const BaseType & operator[](int nIndex) const
		{
			return m_channels[nIndex];
		}

		BaseType & operator[](int nIndex)
		{
			return m_channels[nIndex];
		}

		template <class T>
		void Add(std::string name, const T* data, uint32_t size);
		void Clear();
		bool Contains(VertexChannel* item);
		bool Contains(std::string name);
		int Count();
	private:
		std::vector<VertexChannel*> m_channels;
		VertexContent* m_parent;

		template <class T>
		void Insert(int index, std::string name, const T* data, uint32_t size);

		void ClearAll();
		std::string m_typeid;
	};


	template <class T>
	void VertexChannelCollection::Add(std::string name, const T* data, uint32_t size)
	{
		Insert<T>(m_channels.size(), name, data, size);
	}

	template <class T>
	void VertexChannelCollection::Insert(int index, std::string name, const T* data, uint32_t size)
	{
		VertexChannel* channel = new VertexChannel(name);
		channel->SetTypeOf(typeid(T).name());
		channel->SetSizeOfBase(sizeof(T));
		if (data != nullptr)
		{
			channel->InsertRange<T>(0, data, size);
		}
		else
		{
			std::vector<T> tmpVec(size);
			channel->InsertRange<T>(0, &tmpVec[0], size);
		}
		m_channels.insert(m_channels.begin() + index, channel);
	}
}