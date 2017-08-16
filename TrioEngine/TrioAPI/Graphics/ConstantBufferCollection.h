#pragma once

#include "TrioAPI.h"

#include "ConstantBuffer.h"

#if TRIO_OPENGL
#include "ShaderProgram.h"
#endif

#include "StatesEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL ConstantBufferCollection
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

#ifdef TRIO_DIRECTX
		void SetConstantBuffers(GraphicsDevice* device);
#elif TRIO_OPENGL
		void SetConstantBuffers(GraphicsDevice* device, ShaderProgram* program);
#endif

		void clear();
		void push_back(ConstantBuffer* item);
		inline size_t size();

		friend class GraphicsDevice;
	private:
		ConstantBuffer** m_arrayBuffers;
		ShaderStage m_stage;

		int m_valid;
		int m_count;
	};
}