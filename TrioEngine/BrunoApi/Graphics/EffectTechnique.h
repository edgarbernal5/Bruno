#pragma once

#include "TrioApiRequisites.h"

#include "..\FX\HLSLParser.h"
#include <vector>

namespace BrunoEngine
{
	class EffectPass;
	class GraphicsDevice;
	class Effect;
	
	class BRUNO_API_EXPORT EffectTechnique
	{
	public:
		EffectTechnique(std::vector<EffectPass*>& passes, const char* name);
		EffectTechnique(TrioFX::HLSLTechnique11* technique, TrioFX::HLSLTree& tree, std::vector<TrioFX::HLSLBuffer*> &buffers, std::vector<TrioFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect *effect);
		~EffectTechnique();

		inline const char* GetName() { return m_name.c_str(); }

		//const
		std::vector<EffectPass*>& GetPasses();
		friend class Effect;
	private:

		std::vector<EffectPass*> m_passes;
		std::string m_name;
	};
}