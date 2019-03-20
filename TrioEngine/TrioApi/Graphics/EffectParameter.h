#pragma once

#include "TrioApiDecl.h"

#include "ConstantBufferField.h"
#include "..\Math\MathVector.h"
#include <string>

namespace Vago
{
	class Texture;
	class Effect;
	class ConstantBuffer;
	
	class TRIOAPI_DECL EffectParameter
	{
	public:
		EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect);
		~EffectParameter();

		void SetValue(float value);
		void SetValue(Texture* value);
		void SetValue(Matrix value);
		void SetValue(Vector2 value);
		void SetValue(Vector3 value);
		void SetValue(Vector4 value);

		inline std::string& GetName() { return m_csName; }

		//bool operator <
	private:
#ifdef TRIO_DIRECTX
		//Obsoleto
		//union InternalParameter
		//{
		//	ID3DX11EffectMatrixVariable* m_Matrix;
		//	ID3DX11EffectShaderResourceVariable* m_ShaderResource;
		//	ID3DX11EffectScalarVariable* m_Scalar;
		//	ID3DX11EffectVectorVariable* m_Vector;
		//};
#endif

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
		ConstantBuffer* m_constantBuffer;

		std::string m_csName;
		std::string m_csSemantic;

		Effect *m_pEffect;
		uint32_t m_uiOffset;
		// Tamaño en bytes;
		uint32_t m_uiSize;
	};
}