#pragma once

#include "TrioAPI.h"
#include "ResourceEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL VertexBuffer;

	struct TRIOAPI_DLL VertexBufferBinding
	{
		VertexBuffer* Buffer;
		uint32_t Offset, Stride;

		VertexBufferBinding() : Buffer(nullptr) {}
		VertexBufferBinding(VertexBuffer* buffer, uint32_t offset, uint32_t stride) : Buffer(buffer), Offset(offset), Stride(stride)
		{
		}
	};

	class TRIOAPI_DLL TRIOAPI_DLL VertexBufferBindings
	{
	public:
		VertexBufferBindings();
		~VertexBufferBindings();

		typedef VertexBufferBinding BaseType;

		const BaseType & operator[](int iIndex) const;

		BaseType & operator[](int iIndex);

		void clear();
		void push_back(VertexBufferBinding parameter);
		inline size_t size();

	private:
		BaseType* m_pArray;
		size_t m_iN;
		size_t m_iCapacity;
	};

}

