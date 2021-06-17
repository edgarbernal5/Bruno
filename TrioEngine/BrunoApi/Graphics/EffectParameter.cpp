#include "stdafx.h"
#include "EffectParameter.h"

#include "ConstantBuffer.h"

namespace TrioEngine
{
	EffectParameter::EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect) :
		m_effect(effect),
		m_name(fieldDesc.Name),

		m_data(nullptr)
	{
		m_internalParameter.m_constantBuffer.m_constantBuffer = constantBuffer;
		m_internalParameter.m_constantBuffer.m_offset = fieldDesc.Offset;
		m_internalParameter.m_constantBuffer.m_size = fieldDesc.SizeInBytes;

		m_data = constantBuffer->GetRawData();
	}

	EffectParameter::EffectParameter(STexture textureDesc, std::string parameterName, Effect* effect) :
		m_effect(effect),
		m_name(parameterName),

		m_data(nullptr)
	{
		m_internalParameter.m_texture = textureDesc;

		m_data = &m_internalParameter.m_texture.m_texture;
	}

	EffectParameter::EffectParameter(SSamplerState samplerState, std::string parameterName, Effect* effect) :
		m_effect(effect),
		m_name(parameterName),

		m_data(nullptr)
	{
		m_internalParameter.m_samplerState = samplerState;

		m_data = &m_internalParameter.m_samplerState.m_samplerState;
	}

	EffectParameter::~EffectParameter()
	{
	}

	void EffectParameter::SetValue(Color& value)
	{

	}

	void EffectParameter::SetValue(float value)
	{

	}

	void EffectParameter::SetValue(Texture* value)
	{

	}

	void EffectParameter::SetValue(Matrix& value)
	{
		Matrix transposed = value;
		transposed = transposed.Transpose();

		m_internalParameter.m_constantBuffer.m_constantBuffer->SetInternalData(
			(uint8_t*)&transposed.m[0][0], sizeof(value), m_internalParameter.m_constantBuffer.m_offset
		);
	}

	void EffectParameter::SetValue(Vector2 value)
	{

	}

	void EffectParameter::SetValue(Vector3 value)
	{

	}

	void EffectParameter::SetValue(Vector4 value)
	{

	}
}