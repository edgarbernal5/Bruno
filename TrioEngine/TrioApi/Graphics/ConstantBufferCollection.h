#pragma once

#include "TrioApiDecl.h"

#include "ConstantBuffer.h"

#if TRIO_OPENGL
#include "ShaderProgram.h"
#endif

#include "ShaderStage.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIOAPI_DECL ConstantBufferCollection
	{
	public:
		ConstantBufferCollection();
		ConstantBufferCollection(ShaderStage stage);

		typedef ConstantBuffer* BaseType;

		const BaseType & operator[](int nIndex) const
		{
			return m_arrayBuffers[nIndex];
		}

		BaseType & operator[](int nIndex)
		{
			m_valid |= 1 << nIndex;
			return m_arrayBuffers[nIndex];
		}

		void clear();
		void push_back(ConstantBuffer* item);
		inline size_t size();

		friend class GraphicsDevice;
	private:
		ConstantBuffer* m_arrayBuffers[16];
		ShaderStage m_stage;

		int m_valid;
		int m_count;

#ifdef TRIO_DIRECTX
		void SetConstantBuffers(GraphicsDevice* device);
#endif
#ifdef TRIO_OPENGL
		void SetConstantBuffers(GraphicsDevice* device, ShaderProgram* program);
#endif
		

	};
}