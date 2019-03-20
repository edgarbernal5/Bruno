#pragma once

#include "TrioApiDecl.h"

#include "ConstantBufferField.h"
#include "GraphicsDevice.h"

#include "ShaderStage.h"
#if TRIO_OPENGL
#include "ShaderProgram.h"
#endif

#include <string>

namespace Vago
{
	class TRIOAPI_DECL ConstantBuffer
	{
	public:
		ConstantBuffer(GraphicsDevice* device, std::string name, uint32_t sizeInBytes, std::vector<ConstantBufferField> bufferFields);
		~ConstantBuffer();

		template <typename T>
		void SetData(const T& data, int offsetInBytes);

		void SetData();

#ifdef TRIO_DIRECTX
		void Apply(ShaderStage stage, int slot);
#elif TRIO_OPENGL
		void Apply(ShaderProgram* program);
#endif
		inline size_t GetFieldsCount() { return m_vBufferFields.size(); }

		friend class Effect;
		friend class EffectParameter;
		friend class EffectPass;
		friend class GraphicsDevice;
		friend class ShaderProgramCache;

	private:
		void Initialize();

		std::vector<ConstantBufferField> m_vBufferFields;
		GraphicsDevice* m_pDevice;
		std::string m_csName;
		uint32_t m_uiSizeInBytes;

		uint8_t* m_uiBufferBytes;

		bool m_bDirty;
#ifdef TRIO_DIRECTX
		ID3D11Buffer* m_pBuffer;
		D3D11_BUFFER_DESC m_bufferDesc;

#elif TRIO_OPENGL
		ShaderProgram* m_shaderProgram;
		GLuint m_location;
		GLuint m_buffer;
		GLuint m_uniformBindingIndex;

		static GLuint m_indexTest;
		void InitializeUniformData(ShaderProgram* program);
		 
#endif

		void SetInternalData(uint8_t* data, uint32_t sizeInBytes, int offsetInBytes);
	};

	template <typename T>
	void ConstantBuffer::SetData(const T& data, int offsetInBytes)
	{

#if TRIO_DIRECTX
		if (m_pBuffer == nullptr)
		{
			Initialize();
		}
		//Mejorar esto.
		uint32_t elementSizeInBytes = sizeof(T);
		D3D11_MAPPED_SUBRESOURCE resource;

		m_pDevice->GetD3DDeviceContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		T* pDataResource = (T*)((uint8_t*)(resource.pData) + offsetInBytes);
		
		memcpy(pDataResource, &data, elementSizeInBytes);

		m_pDevice->GetD3DDeviceContext()->Unmap(m_pBuffer, 0);
#elif TRIO_OPENGL
		//if (m_buffer == 0)
		//{
		//	Initialize();
		//}

		//glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
		//glBufferSubData(GL_UNIFORM_BUFFER, offsetInBytes, sizeof(T), (&data));
		//
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif
	}

	
}
