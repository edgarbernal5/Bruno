#include "stdafx.h"
#include "EffectParameter.h"

#include "ConstantBuffer.h"

namespace Vago
{
	EffectParameter::EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect) :
		m_pEffect(effect),
		m_csName(fieldDesc.Name)
	{
		m_uInternalParameter.m_ConstantBuffer.m_pConstantBuffer = constantBuffer;
		m_uInternalParameter.m_ConstantBuffer.m_uiOffset = fieldDesc.Offset;
		m_uInternalParameter.m_ConstantBuffer.m_uiSize = fieldDesc.SizeInBytes;
	}

	EffectParameter::EffectParameter(STexture textureDesc, std::string parameterName, Effect* effect) :
		m_pEffect(effect),
		m_csName(parameterName)
	{

	}

	EffectParameter::~EffectParameter()
	{
	}


	void EffectParameter::SetValue(float value)
	{

	}

	void EffectParameter::SetValue(Texture* value)
	{

	}

	void EffectParameter::SetValue(Matrix value)
	{
		Matrix transposed = value;
		transposed = transposed.Transpose();
		m_uInternalParameter.m_ConstantBuffer.m_pConstantBuffer->SetInternalData((uint8_t*)&transposed.m[0][0], sizeof(value), m_uInternalParameter.m_ConstantBuffer.m_uiOffset);
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