#include "stdafx.h"
#include "EffectParameter.h"

#include "ConstantBuffer.h"

namespace Vago
{
	EffectParameter::EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect) :
		m_pEffect(effect),
		m_constantBuffer(constantBuffer)
	{
		m_csName = fieldDesc.Name;
		m_uiOffset = fieldDesc.Offset;
		m_uiSize = fieldDesc.SizeInBytes;
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
		m_constantBuffer->SetInternalData((uint8_t*)&transposed.m[0][0], sizeof(value), m_uiOffset);
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