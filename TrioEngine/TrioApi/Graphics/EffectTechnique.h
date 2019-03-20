#pragma once

#include "TrioApiDecl.h"

#include "..\FX\HLSLParser.h"
#include <vector>

namespace Vago
{
	class EffectPass;
	class GraphicsDevice;
	class Effect;
	
	class TRIOAPI_DECL EffectTechnique
	{
	public:
		EffectTechnique();
		EffectTechnique(std::vector<EffectPass*>& passes, const char* name);
		EffectTechnique(TrioFX::HLSLTechnique11* technique, TrioFX::HLSLTree* tree, GraphicsDevice* device, Effect *effect);
		~EffectTechnique();

		inline const char* GetName() { return m_csName.c_str(); }

		//const
		std::vector<EffectPass*>& GetPasses();
		friend class Effect;
	private:

		std::vector<EffectPass*> m_vPasses;
		std::string m_csName;
	};
}