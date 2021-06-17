#include "stdafx.h"
#include "ConstantBuffer.h"

namespace BrunoEngine
{
#if OPENGL
	GLuint ConstantBuffer::m_indexTest = 0;
#endif
	ConstantBuffer::ConstantBuffer(GraphicsDevice* device, const std::string& name, uint32_t sizeInBytes, std::vector<ConstantBufferField>& bufferFields) :
		m_device(device),
#ifdef BRUNO_DIRECTX
		m_buffer(nullptr),
#elif OPENGL
		m_buffer(0),
		m_uniformBindingIndex(0),
#endif
		m_sizeInBytes(sizeInBytes),
		
		m_bufferFields(bufferFields),
		m_name(name)
	{

		m_bufferBytes = new uint8_t[sizeInBytes];
	}

	ConstantBuffer::ConstantBuffer(GraphicsDevice* device, const std::string& name) :
		m_device(device),
#ifdef BRUNO_DIRECTX
		m_buffer(nullptr),
#elif OPENGL
		m_buffer(0),
		m_uniformBindingIndex(0),
#endif
		m_name(name)
	{

	}

	ConstantBuffer::~ConstantBuffer()
	{
		if (m_bufferBytes != nullptr){
			delete m_bufferBytes;
			m_bufferBytes = nullptr;
		}
	}

#ifdef BRUNO_DIRECTX
	//ConstantBuffer::ConstantBuffer(ID3DX11EffectConstantBuffer* dxbuffer, GraphicsDevice* device) :
	//	m_Device(device), m_buffer(nullptr)
	//{
	//	dxbuffer->GetDesc(&m_Desc);

	//	ID3DX11EffectType * type = dxbuffer->GetType();
	//	D3DX11_EFFECT_TYPE_DESC ptypeDesc;
	//	type->GetDesc(&ptypeDesc);

	//	m_Name = m_Desc.Name;
	//}
#endif

	void ConstantBuffer::Initialize()
	{
#if BRUNO_DIRECTX
		m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_bufferDesc.MiscFlags = 0;
		m_bufferDesc.ByteWidth = static_cast<uint32_t>(m_sizeInBytes + (16 - (m_sizeInBytes % 16))); //multiplo de 16 bytes, que es el tamaño del registro ( c_RegisterSize = c_ScalarsPerRegister * c_ScalarSize )
		//_declspec(align(16))
		//https://docs.microsoft.com/es-es/windows/desktop/direct3dhlsl/dx-graphics-hlsl-packing-rules
		//m_bufferDesc.ByteWidth = m_sizeInBytes;

		DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateBuffer(&m_bufferDesc, nullptr, &m_buffer));
#elif BRUNO_OPENGL
		////m_sizeInBytes = static_cast<uint32_t>(m_sizeInBytes+ (m_sizeInBytes % 16));
		//glGenBuffers(1, &m_buffer);
		//glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
		//glBufferData(GL_UNIFORM_BUFFER, m_sizeInBytes, nullptr, GL_STREAM_DRAW);
		//glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_buffer, 0, m_sizeInBytes);
		
#endif
	}

	void ConstantBuffer::SetData()
	{
#if BRUNO_DIRECTX
		if (m_buffer == nullptr)
		{
			Initialize();
		}
		//Mejorar esto.
		uint32_t elementSizeInBytes = m_sizeInBytes;
		D3D11_MAPPED_SUBRESOURCE resource;

		m_device->GetD3DDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		uint8_t* pDataResource = (uint8_t*)(resource.pData);

		memcpy(pDataResource, m_bufferBytes, elementSizeInBytes);

		m_device->GetD3DDeviceContext()->Unmap(m_buffer, 0);
#endif
	}

#if BRUNO_DIRECTX
	void ConstantBuffer::Apply(ShaderStage stage, int slot)
	{
		if (m_buffer == nullptr)
		{
			Initialize();
		}

		if (m_dirty)
		{
			SetData();
			m_dirty = false;
		}

		switch (stage)
		{
		case ShaderStage::Vertex:
			m_device->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderStage::Pixel:
			m_device->GetD3DDeviceContext()->PSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderStage::Geometry:
			break;
		case ShaderStage::Compute:
			break;
		default:
			break;
		}
	}
#elif BRUNO_OPENGL

	void ConstantBuffer::InitializeUniformData(ShaderProgram* program)
	{
		m_location = glGetUniformBlockIndex(program->GetProgram(), m_Name.c_str());
		//m_uniformBindingIndex = m_indexTest++;
		glUniformBlockBinding(program->GetProgram(), m_location, m_uniformBindingIndex);
	}

	void ConstantBuffer::Apply(ShaderProgram* program)
	{
		if (m_buffer == 0)
		{
			Initialize();
		}
		if (m_dirty)
		{
			for (size_t i = 0; i < m_bufferFields.size(); i++)
			{
				ConstantBufferField& field = m_bufferFields[i];
				GLuint location = program->GetUniformLocation(field.Name);
				if (location >= 0)
				{
					if (field.BaseTypeName == "Float4x4"){
						glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)(m_bufferBytes + field.Offset));
					}
					else if (field.BaseTypeName == "Float3"){
						glUniform3fv(location, 1, (const float*)(m_bufferBytes + field.Offset));
					}
					else if (field.BaseTypeName == "Float4"){
						glUniform4fv(location, 1, (const float*)(m_bufferBytes + field.Offset));
					}
					else if (field.BaseTypeName == "Float2"){
						glUniform2fv(location, 1, (const float*)(m_bufferBytes + field.Offset));
					}
				}
			}

			m_dirty = false;
		}
		if (m_shaderProgram != program){
			

			//GLuint temp = glGetUniformBlockIndex(program->GetProgram(), m_Name.c_str());
			
			//GLuint location = program->GetUniformLocation(m_Name);
			//if (location == -1)
			//	return;

			m_shaderProgram = program;
			//m_location = location;
			//m_dirty = true;
		}

		//m_dirty = false;
	}
#endif

	void ConstantBuffer::SetInternalData(uint8_t* data, uint32_t sizeInBytes, int offsetInBytes)
	{
		memcpy(m_bufferBytes + offsetInBytes, data, sizeInBytes);
		m_dirty = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}