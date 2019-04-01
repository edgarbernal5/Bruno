#include "stdafx.h"
#include "ConstantBuffer.h"

namespace Vago
{
#if OPENGL
	GLuint ConstantBuffer::m_indexTest = 0;
#endif
	ConstantBuffer::ConstantBuffer(GraphicsDevice* device, std::string name, uint32_t sizeInBytes, std::vector<ConstantBufferField>& bufferFields) :
		m_pDevice(device),
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif OPENGL
		m_buffer(0),
		m_uniformBindingIndex(0),
#endif
		m_uiSizeInBytes(sizeInBytes),
		
		m_vBufferFields(bufferFields),
		m_csName(name)
	{

		m_uiBufferBytes = new uint8_t[sizeInBytes];
	}

	ConstantBuffer::ConstantBuffer(GraphicsDevice* device, std::string name) :
		m_pDevice(device),
#ifdef TRIO_DIRECTX
		m_pBuffer(nullptr),
#elif OPENGL
		m_buffer(0),
		m_uniformBindingIndex(0),
#endif
		m_csName(name)
	{

	}

	ConstantBuffer::~ConstantBuffer()
	{
		if (m_uiBufferBytes != nullptr){
			delete m_uiBufferBytes;
			m_uiBufferBytes = nullptr;
		}
	}

#ifdef TRIO_DIRECTX
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
#if TRIO_DIRECTX
		m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_bufferDesc.MiscFlags = 0;
		m_bufferDesc.ByteWidth = static_cast<uint32_t>(m_uiSizeInBytes + (16 - (m_uiSizeInBytes % 16))); //multiplo de 16 bytes, que es el tama�o del registro ( c_RegisterSize = c_ScalarsPerRegister * c_ScalarSize )
		//_declspec(align(16))
		//https://docs.microsoft.com/es-es/windows/desktop/direct3dhlsl/dx-graphics-hlsl-packing-rules
		//m_bufferDesc.ByteWidth = m_sizeInBytes;

		DX::ThrowIfFailed(m_pDevice->GetD3DDevice()->CreateBuffer(&m_bufferDesc, nullptr, &m_pBuffer));
#elif TRIO_OPENGL
		////m_sizeInBytes = static_cast<uint32_t>(m_sizeInBytes+ (m_sizeInBytes % 16));
		//glGenBuffers(1, &m_buffer);
		//glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
		//glBufferData(GL_UNIFORM_BUFFER, m_sizeInBytes, nullptr, GL_STREAM_DRAW);
		//glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_buffer, 0, m_sizeInBytes);
		
#endif
	}

	void ConstantBuffer::SetData()
	{
#if TRIO_DIRECTX
		if (m_pBuffer == nullptr)
		{
			Initialize();
		}
		//Mejorar esto.
		uint32_t elementSizeInBytes = m_uiSizeInBytes;
		D3D11_MAPPED_SUBRESOURCE resource;

		m_pDevice->GetD3DDeviceContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		uint8_t* pDataResource = (uint8_t*)(resource.pData);

		memcpy(pDataResource, m_uiBufferBytes, elementSizeInBytes);

		m_pDevice->GetD3DDeviceContext()->Unmap(m_pBuffer, 0);
#endif
	}

#if TRIO_DIRECTX
	void ConstantBuffer::Apply(ShaderStage stage, int slot)
	{
		if (m_pBuffer == nullptr)
		{
			Initialize();
		}

		if (m_bDirty)
		{
			SetData();
			m_bDirty = false;
		}

		switch (stage)
		{
		case ShaderStage::Vertex:
			m_pDevice->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, &m_pBuffer);
			break;
		case ShaderStage::Pixel:
			m_pDevice->GetD3DDeviceContext()->PSSetConstantBuffers(slot, 1, &m_pBuffer);
			break;
		case ShaderStage::Geometry:
			break;
		case ShaderStage::Compute:
			break;
		default:
			break;
		}
	}
#elif TRIO_OPENGL

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
		memcpy(m_uiBufferBytes + offsetInBytes, data, sizeInBytes);
		m_bDirty = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}