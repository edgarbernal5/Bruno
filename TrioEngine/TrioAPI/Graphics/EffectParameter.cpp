#include "stdafx.h"
#include "EffectParameter.h"

#include "Effect.h"
#include "Texture.h"

namespace Cuado
{
	EffectParameter::EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect) :
		m_Effect(effect),
		m_constantBuffer(constantBuffer)
	{
		m_Name = fieldDesc.Name;
		m_offset = fieldDesc.Offset;
		m_size = fieldDesc.SizeInBytes;
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
		m_constantBuffer->SetInternalData((uint8_t*)&transposed.m[0][0], sizeof(value), m_offset);
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