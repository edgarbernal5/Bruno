#pragma once

#include "TrioAPI.h"

#include "GraphicsDevice.h"
#if TRIO_OPENGL
#include "ShaderProgram.h"
#endif

#include "StatesEnums.h"
#include <string>

namespace Cuado
{
	struct TRIOAPI_DLL ConstantBufferField
	{
		std::string Name;
		uint32_t Offset;
		uint32_t SizeInBytes;
		std::string BaseTypeName;

		ConstantBufferField() {}
		ConstantBufferField(std::string _name, uint32_t _offset, uint32_t _size, std::string _baseName) :
			Name(_name), Offset(_offset), SizeInBytes(_size), BaseTypeName(_baseName) {}
	};

	class TRIOAPI_DLL ConstantBuffer
	{
	public:
		ConstantBuffer(GraphicsDevice* device, std::string name, uint32_t sizeInBytes, std::vector<ConstantBufferField> bufferFields);
		~ConstantBuffer();

		template <typename T>
		void SetData(const T& data, int offsetInBytes);

		void SetData();

#ifdef TRIO_DIRECTX
		void Apply(ShaderStage stage, int slot);
#elif OPENGL
		void Apply(ShaderProgram* program);
#endif
		inline size_t GetFieldsCount() { return m_bufferFields.size(); }

		friend class Effect;
		friend class EffectParameter;
		friend class EffectPass;
		friend class GraphicsDevice;
		friend class ShaderProgramCache;

	private:
#ifdef TRIO_DIRECTX
		//ConstantBuffer(ID3DX11EffectConstantBuffer* dxbuffer, GraphicsDevice* device);
#endif
		void Initialize();

		std::vector<ConstantBufferField> m_bufferFields;
		GraphicsDevice* m_Device;
		std::string m_Name;
		uint32_t m_sizeInBytes;

		uint8_t* m_bufferBytes;

		bool m_dirty;
#ifdef TRIO_DIRECTX
		ID3D11Buffer* m_buffer;
		D3D11_BUFFER_DESC m_bufferDesc;

		//Efecto.
		//D3DX11_EFFECT_VARIABLE_DESC m_Desc;
#elif OPENGL
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
		if (m_buffer == nullptr)
		{
			Initialize();
		}
		//Mejorar esto.
		uint32_t elementSizeInBytes = sizeof(T);
		D3D11_MAPPED_SUBRESOURCE resource;

		m_Device->GetD3DDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		T* pDataResource = (T*)((uint8_t*)(resource.pData) + offsetInBytes);
		
		memcpy(pDataResource, &data, elementSizeInBytes);

		m_Device->GetD3DDeviceContext()->Unmap(m_buffer, 0);
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
