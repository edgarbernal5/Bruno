#pragma once

#include "TrioApiDecl.h"

namespace TrioEngine
{
	class VertexBuffer;

	struct TRIOAPI_DECL VertexBufferBinding
	{
		VertexBuffer* Buffer;
		uint32_t Offset, Stride;

		VertexBufferBinding() : Buffer(nullptr), Offset(0), Stride(0) {}
		VertexBufferBinding(VertexBuffer* buffer, uint32_t offset, uint32_t stride) : Buffer(buffer), Offset(offset), Stride(stride)
		{
		}
		VertexBufferBinding(const VertexBufferBinding& rhs)
		{
			Buffer = rhs.Buffer;
			Offset = rhs.Offset;
			Stride = rhs.Stride;
		}
	};

	struct TRIOAPI_DECL VertexBufferBindings
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
		BaseType m_pArray[32];
		size_t m_iN;
		size_t m_iCapacity;
	};

}

