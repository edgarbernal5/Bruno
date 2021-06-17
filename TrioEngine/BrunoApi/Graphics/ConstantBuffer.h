#pragma once

#include "TrioApiRequisites.h"

#include "ConstantBufferField.h"
#include "GraphicsDevice.h"

#include "ShaderStage.h"
#if BRUNO_OPENGL
#include "ShaderProgram.h"
#endif

#include <string>

namespace BrunoEngine
{
	class BRUNO_API_EXPORT ConstantBuffer
	{
	public:
		ConstantBuffer(GraphicsDevice* device, const std::string& name, uint32_t sizeInBytes, std::vector<ConstantBufferField>& bufferFields);
		ConstantBuffer(GraphicsDevice* device, const std::string& name);
		~ConstantBuffer();

		inline uint8_t* GetRawData() {
			return m_bufferBytes;
		}

		template <typename T>
		void SetData(const T& data, int offsetInBytes);

		void SetData();

#ifdef BRUNO_DIRECTX
		void Apply(ShaderStage stage, int slot);
#elif BRUNO_OPENGL
		void Apply(ShaderProgram* program);
#endif
		inline std::vector<ConstantBufferField>& GetFieldsDesc() { return m_bufferFields; }

		friend class Effect;
		friend class EffectLoader;
		friend class EffectParameter;
		friend class EffectPass;
		friend class GraphicsDevice;
		friend class ShaderProgramCache;

	private:
		void Initialize();

		std::vector<ConstantBufferField> m_bufferFields;
		GraphicsDevice* m_device;
		std::string m_name;
		uint32_t m_sizeInBytes;

		uint8_t* m_bufferBytes;

		bool m_dirty;
#ifdef BRUNO_DIRECTX
		ID3D11Buffer* m_buffer;
		D3D11_BUFFER_DESC m_bufferDesc;

#elif BRUNO_OPENGL
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
#if BRUNO_DIRECTX
		if (m_buffer == nullptr)
		{
			Initialize();
		}
		//Mejorar esto.
		uint32_t elementSizeInBytes = sizeof(T);
		D3D11_MAPPED_SUBRESOURCE resource;

		m_device->GetD3DDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		T* pDataResource = (T*)((uint8_t*)(resource.pData) + offsetInBytes);
		
		memcpy(pDataResource, &data, elementSizeInBytes);

		m_device->GetD3DDeviceContext()->Unmap(m_buffer, 0);
#elif BRUNO_OPENGL
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
