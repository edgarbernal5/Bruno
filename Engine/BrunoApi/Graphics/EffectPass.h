#pragma once

#include "TrioApiRequisites.h"

#include "..\FX\HLSLParser.h"
#include <string>

namespace BrunoEngine
{
	class GraphicsDevice;
	class Effect;
	class Shader;
	
	class BRUNO_API_EXPORT EffectPass
	{
	public:
		EffectPass(BrunoFX::HLSLPass11* pass11, BrunoFX::HLSLTree& tree, std::vector<BrunoFX::HLSLBuffer*> &buffers, std::vector<BrunoFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect* effect);
		EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect);

		void Apply();

		inline const char* GetName() { return m_name.c_str(); }

	private:
		Effect* m_effect;
		Shader* m_vertexShader;
		Shader* m_pixelShader;

		GraphicsDevice* m_device;

		std::string m_name;
	};
}