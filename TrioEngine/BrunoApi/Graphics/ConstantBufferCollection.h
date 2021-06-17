#pragma once

#include "TrioApiRequisites.h"

#include "ConstantBuffer.h"

#if BRUNO_OPENGL
#include "ShaderProgram.h"
#endif

#include "ShaderStage.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class BRUNO_API_EXPORT ConstantBufferCollection
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

#ifdef BRUNO_DIRECTX
		void SetConstantBuffers(GraphicsDevice* device);
#endif
#ifdef BRUNO_OPENGL
		void SetConstantBuffers(GraphicsDevice* device, ShaderProgram* program);
#endif
		

	};
}