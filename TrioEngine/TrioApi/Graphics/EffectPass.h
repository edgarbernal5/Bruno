#pragma once

#include "TrioApiDecl.h"

#include "..\FX\HLSLParser.h"
#include <string>

namespace Vago
{
	class GraphicsDevice;
	class Effect;
	class Shader;
	
	class TRIOAPI_DECL EffectPass
	{
	public:
		EffectPass();
		EffectPass(TrioFX::HLSLPass11* pass, TrioFX::HLSLTree* tree, GraphicsDevice* device, Effect* effect);
		EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect);

		void Apply();

		inline const char* GetName() { return m_csName.c_str(); }
	private:

		Effect* m_pEffect;
		Shader* m_pVertexShader;
		Shader* m_pPixelShader;

		GraphicsDevice* m_pDevice;

		std::string m_csName;
	};
}