#pragma once

#include "TrioAPI.h"

#include <string>
#include "SimpleMath.h"
#include "ConstantBuffer.h"

namespace Cuado
{
	class TRIOAPI_DLL Texture;
	class TRIOAPI_DLL Effect;

	class TRIOAPI_DLL EffectParameter
	{
	public:
#ifdef TRIO_DIRECTX
		//EffectParameter(ID3DX11EffectVariable* variable, Effect* effect);
#endif
		EffectParameter(ConstantBufferField fieldDesc, ConstantBuffer* constantBuffer, Effect* effect);
		~EffectParameter();

		void SetValue(float value);
		void SetValue(Texture* value);
		void SetValue(Matrix value);
		void SetValue(Vector2 value);
		void SetValue(Vector3 value);
		void SetValue(Vector4 value);

		inline std::string& GetName() { return m_Name; }

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

		std::string m_Name;
		std::string m_Semantic;

		Effect *m_Effect;
		uint32_t m_offset;
		// Tamaño en bytes;
		uint32_t m_size;
	};
}