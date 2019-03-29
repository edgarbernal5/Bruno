#pragma once

#include "TrioApiDecl.h"

#include "ConstantBufferField.h"
#include "..\Math\MathVector.h"
#include <string>

#include "TextureAddressMode.h"
#include "TextureFilter.h"

namespace Vago
{
	class Texture;
	class Effect;
	class ConstantBuffer;
	class SamplerState;
	
	class TRIOAPI_DECL EffectParameter
	{
	private:
		struct STexture {
			Texture* m_pTexture;
			int textureSlot;
		};
		struct SSamplerState {
			SamplerState* m_pSamplerState;

		};
		struct SConstantBuffer {
			ConstantBuffer* m_pConstantBuffer;
			uint32_t m_uiOffset;
			// Tamaño en bytes;
			uint32_t m_uiSize;
		};

	public:
		EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect);
		EffectParameter(STexture textureDesc, std::string parameterName, Effect* effect);
		~EffectParameter();

		void SetValue(float value);
		void SetValue(Texture* value);
		void SetValue(Matrix value);
		void SetValue(Vector2 value);
		void SetValue(Vector3 value);
		void SetValue(Vector4 value);

		inline std::string& GetName() { return m_csName; }

		friend class Effect;
		//bool operator <
	private:

		union InternalDataPointer
		{
			STexture m_Texture;
			SConstantBuffer m_ConstantBuffer;
			SSamplerState m_SamplerState;

		//	ID3DX11EffectMatrixVariable* m_Matrix;
		//	ID3DX11EffectShaderResourceVariable* m_ShaderResource;
		//	ID3DX11EffectScalarVariable* m_Scalar;
		//	ID3DX11EffectVectorVariable* m_Vector;
		};

		/*union InternalValue
		{
		Matrix m_matrix4x4;
		Float3x3 m_matrix3x3;
		};
		*/


#ifdef TRIO_DIRECTX
		//ID3DX11EffectVariable* m_Variable;

		//D3DX11_EFFECT_TYPE_DESC m_Typedesc;
		//InternalParameter m_internalParameter;

#endif
		InternalDataPointer m_uInternalParameter;
		
		std::string m_csName;
		std::string m_csSemantic;

		Effect *m_pEffect;
	};
}