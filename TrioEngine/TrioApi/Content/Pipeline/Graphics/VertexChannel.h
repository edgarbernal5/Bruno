#pragma once

#include "TrioApiRequisites.h"
#include "Errors/ContentPipelineException.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT VertexChannel
	{
	public:
		typedef uint8_t* BaseType;

		VertexChannel(std::string name);
		~VertexChannel();

		const BaseType & operator[](int nIndex) const
		{
			//return (BaseType)(&(m_items[nIndex]));
			return (&(m_rawData[nIndex * m_sizeInBytesOfT]));
		}

		inline const char* GetName()
		{
			return m_name.c_str();
		}

		inline int Count()
		{
			return m_itemsSize;
			//return m_items.size() / sizeof(uint8_t);
		}
		bool Contains(void* value)
		{
			return false;
		}

		void Add(void* value)
		{
			//m_items.push_back(value);
		}

		void Clear()
		{
			//TO-DO: limpiar memoria.
			//m_items.clear();
			m_itemsSize = 0;
		}

		void SetTypeOf(std::string typeId)
		{
			m_typeId = typeId;
		}

		inline const std::string& GetTypeOf()
		{
			return m_typeId;
		}

		void SetSizeOfBase(int sizeInBytes)
		{
			m_sizeInBytesOfT = sizeInBytes;
		}

		inline int GetSizeOfBase()
		{
			return m_sizeInBytesOfT;
		}

		template <class T>
		void InsertRange(int index, const T* data, uint32_t size);

	private:
		std::string m_name;

		uint32_t m_itemsSize;

		uint8_t* m_rawData;
		size_t m_rawDataSize;

		std::string m_typeId;
		int m_sizeInBytesOfT;
	};

	template <class T>
	void VertexChannel::InsertRange(int index, const T* data, uint32_t size)
	{
		if (size > 0)
		{
			int stride = sizeof(T);
			if (m_sizeInBytesOfT > 0 && stride != m_sizeInBytesOfT)
			{
				throw ContentPipelineException("");
			}

			const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(data);
			if (m_rawDataSize == 0)
			{
				m_rawDataSize = stride * size;
				uint8_t* cloneDataBytes = new uint8_t[m_rawDataSize];
				memcpy(cloneDataBytes, dataBytes, m_rawDataSize);

				m_rawData = cloneDataBytes;
			}
			else
			{
				size_t newrawDataSize = stride * size;
				uint8_t* newcloneDataBytes = new uint8_t[newrawDataSize + m_rawDataSize];

				memcpy(newcloneDataBytes + index * stride + newrawDataSize, m_rawData, m_rawDataSize);
				memcpy(newcloneDataBytes + index * stride, dataBytes, newrawDataSize);

				delete[] m_rawData;

				m_rawDataSize = newrawDataSize;
				m_rawData = newcloneDataBytes;
			}

			m_itemsSize += size;
		}
	}
}