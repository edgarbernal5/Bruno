#pragma once

#include <map>
#include <string>

namespace TrioEngine
{
	class OpaqueData
	{
	private:
		struct Data {
			uint32_t m_sizeInBytes;
			uint8_t* m_bytes;

			Data(uint32_t sizeInBytes, uint8_t* bytes) : m_sizeInBytes(sizeInBytes), m_bytes(bytes){}
			Data() : m_sizeInBytes(0), m_bytes(nullptr) {}
		};

	public:
		typedef typename std::map<std::string, Data>::iterator iterator;
		typedef typename std::map<std::string, Data>::const_iterator const_iterator;

		OpaqueData();
		~OpaqueData();

		void addProperties(const_iterator begin, const_iterator end);

		template <class T>
		T* get(std::string key);

		template <class T>
		void set(std::string key, const T& value);

		iterator begin() { return m_table.begin(); }
		iterator end() { return m_table.end(); }
		const_iterator begin() const { return m_table.begin(); }
		const_iterator end() const { return m_table.end(); }

	private:
		std::map<std::string, Data> m_table;
	};

	template <class T>
	T* OpaqueData::get(std::string key)
	{
		auto it = m_table.find(key);
		if (it == m_table.end())
		{
			return nullptr;
		}
		return reinterpret_cast<T*>(it->second.m_bytes);
	}


	template <class T>
	void OpaqueData::set(std::string key, const T& value)
	{
		uint32_t sizeInBytes = sizeof(T);
		auto it = m_table.find(key);
		if (it == m_table.end())
		{
			T* newItem = new T(value);
			
			Data data(sizeInBytes, reinterpret_cast<uint8_t*>(newItem));

			m_table[key] = data;
			return;
		}
		T* oldItem = reinterpret_cast<T*>(it->second.m_bytes);
		delete oldItem;

		T* newItem = new T(value);
		Data data(sizeInBytes, reinterpret_cast<uint8_t*>(newItem));
		m_table[key] = data;
	}
}