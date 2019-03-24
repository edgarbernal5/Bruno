#pragma once

#include "../FX/AssertFX.h"
#include "../Memory/Allocator.h"

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
		Array(TrioMem::Allocator * allocator) : allocator(allocator), buffer(nullptr), size(0), capacity(0) {}

		void PushBack(const T & val)
		{
			ASSERT(&val < buffer || &val >= buffer + size);

			int old_size = size;
			int new_size = size + 1;

			SetSize(new_size);

			ConstructRange(buffer, new_size, old_size, val);
		}

		T & PushBackNew()
		{
			int old_size = size;
			int new_size = size + 1;

			SetSize(new_size);

			ConstructRange(buffer, new_size, old_size);

			return buffer[old_size];
		}
		
		void Resize(int new_size)
		{
			int old_size = size;

			DestroyRange(buffer, new_size, old_size);

			SetSize(new_size);

			ConstructRange(buffer, new_size, old_size);
		}

		int GetSize() const { return size; }
		const T & operator[](int i) const { ASSERT(i < size); return buffer[i]; }
		T & operator[](int i) { ASSERT(i < size); return buffer[i]; }

	private:

		// Change array size.
		void SetSize(int new_size) {
			size = new_size;

			if (new_size > capacity) {
				int new_buffer_size;
				if (capacity == 0) {
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
			ASSERT(new_capacity >= size);

			if (new_capacity == 0)
			{
				// free the buffer.
				if (buffer != NULL)
				{
					allocator->Delete<T>(buffer);
					buffer = NULL;
				}
			}
			else
			{
				// realloc the buffer
				buffer = allocator->Realloc<T>(buffer, new_capacity);
			}

			capacity = new_capacity;
		}


	private:
		TrioMem::Allocator * allocator; // @@ Do we really have to keep a pointer to this?
		T * buffer;
		int size;
		int capacity;
	};
}