#pragma once

#include "TrioAPI.h"

#include "../../TrioFX/HLSLParser.h"
using namespace CuadoFX;

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL Effect;
	class TRIOAPI_DLL Shader;

	class TRIOAPI_DLL EffectPass
	{
	public:
		EffectPass();
		EffectPass(HLSLPass11* pass, HLSLTree* tree, GraphicsDevice* device, Effect* effect);
		EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect);

		void Apply();

		inline const std::string& GetName() { return m_Name; }

		friend class GraphicsDevice;
	private:

		Effect* m_Effect;
		Shader* m_vertexShader;
		Shader* m_pixelShader;

		GraphicsDevice* m_pDevice;

		std::string m_Name;
	};
}