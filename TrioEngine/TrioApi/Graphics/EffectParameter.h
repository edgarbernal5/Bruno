#pragma once

#include "TrioApiRequisites.h"

#include "ConstantBufferField.h"
#include "..\Math\MathVector.h"
#include <string>

#include "TextureAddressMode.h"
#include "TextureFilter.h"

namespace TrioEngine
{
	class Texture;
	class Effect;
	class ConstantBuffer;
	class SamplerState;
	
	class TRIO_API_EXPORT EffectParameter
	{
	private:
		struct STexture {
			Texture* m_texture;
			int textureSlot;
		};
		struct SSamplerState {
			SamplerState* m_samplerState;
			int samplerSlot;

		};
		struct SConstantBuffer {
			ConstantBuffer* m_constantBuffer;
			uint32_t m_offset;
			// Tamaño en bytes;
			uint32_t m_size;
		};

	public:
		EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect);
		EffectParameter(STexture textureDesc, std::string parameterName, Effect* effect);
		EffectParameter(SSamplerState samplerState, std::string parameterName, Effect* effect);
		~EffectParameter();

		void SetValue(float value);
		void SetValue(Texture* value);
		void SetValue(Matrix value);
		void SetValue(Vector2 value);
		void SetValue(Vector3 value);
		void SetValue(Vector4 value);

		inline const char* GetName() { return m_name.c_str(); }

		friend class Effect;
		friend class EffectLoader;
		//bool operator <
	private:

		union InternalDataPointer
		{
			STexture m_texture;
			SConstantBuffer m_constantBuffer;
			SSamplerState m_samplerState;
		};

		void* m_data;

#ifdef TRIO_DIRECTX
		//ID3DX11EffectVariable* m_Variable;

		//D3DX11_EFFECT_TYPE_DESC m_Typedesc;
		//InternalParameter m_internalParameter;

#endif
		InternalDataPointer m_internalParameter;
		
		std::string m_name;
		std::string m_semantic;

		Effect *m_effect;
	};
}