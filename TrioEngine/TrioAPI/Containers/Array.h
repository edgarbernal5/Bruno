#pragma once

#include "FX/AssertFX.h"
#include "Memory/Allocator.h"

namespace TrioData
{
	template <typename T>
	void ConstructRange(T * buffer, int new_size, int old_size) {
		for (int i = old_size; i < new_size; i++) {
			new(buffer + i) T; // placement new
		}
	}

	template <typename T>
	void ConstructRange(T * buffer, int new_size, int old_size, const T & val) {
		for (int i = old_size; i < new_size; i++) {
			new(buffer + i) T(val); // placement new
		}
	}

	template <typename T>
	void DestroyRange(T * buffer, int new_size, int old_size) {
		for (int i = new_size; i < old_size; i++) {
			(buffer + i)->~T(); // Explicit call to the destructor
		}
	}

	template <typename T>
	class Array {
	public:
		Array(TrioMem::Allocator * allocator) : 
			m_pAllocator(allocator),
			m_pBuffer(nullptr), 
			m_iSize(0), 
			m_iCapacity(0) {}

		void PushBack(const T & val)
		{
			ASSERT(&val < m_pBuffer || &val >= m_pBuffer + m_iSize);

			int old_size = m_iSize;
			int new_size = m_iSize + 1;

			SetSize(new_size);

			ConstructRange(m_pBuffer, new_size, old_size, val);
		}

		T & PushBackNew()
		{
			int old_size = m_iSize;
			int new_size = m_iSize + 1;

			SetSize(new_size);

			ConstructRange(m_pBuffer, new_size, old_size);

			return m_pBuffer[old_size];
		}
		
		void Resize(int new_size)
		{
			int old_size = m_iSize;

			DestroyRange(m_pBuffer, new_size, old_size);

			SetSize(new_size);

			ConstructRange(m_pBuffer, new_size, old_size);
		}

		int GetSize() const { return m_iSize; }
		const T & operator[](int i) const { ASSERT(i < m_iSize); return m_pBuffer[i]; }
		T & operator[](int i) { ASSERT(i < m_iSize); return m_pBuffer[i]; }

	private:

		// Change array size.
		void SetSize(int new_size) {
			m_iSize = new_size;

			if (new_size > m_iCapacity) {
				int new_buffer_size;
				if (m_iCapacity == 0) {
					// first allocation is exact
					new_buffer_size = new_size;
				}
				else {
					// following allocations grow array by 25%
					new_buffer_size = new_size + (new_size >> 2);
				}

				SetCapacity(new_buffer_size);
			}
		}

		// Change array capacity.
		void SetCapacity(int new_capacity)
		{
			ASSERT(new_capacity >= m_iSize);

			if (new_capacity == 0)
			{
				// free the buffer.
				if (m_pBuffer != nullptr)
				{
					m_pAllocator->Delete<T>(m_pBuffer);
					m_pBuffer = nullptr;
				}
			}
			else
			{
				// realloc the buffer
				m_pBuffer = m_pAllocator->Realloc<T>(m_pBuffer, new_capacity);
			}

			m_iCapacity = new_capacity;
		}


	private:
		TrioMem::Allocator * m_pAllocator; // @@ Do we really have to keep a pointer to this?
		T * m_pBuffer;
		int m_iSize;
		int m_iCapacity;
	};
}