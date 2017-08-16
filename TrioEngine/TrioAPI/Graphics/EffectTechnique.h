#pragma once

#include "TrioAPI.h"

#include "../../TrioFX/HLSLParser.h"
using namespace CuadoFX;

namespace Cuado
{
	class TRIOAPI_DLL EffectPass;
	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL Effect;

	class TRIOAPI_DLL EffectTechnique
	{
	public:
		EffectTechnique();
		EffectTechnique(vector<EffectPass*>& passes, const char* name);
		EffectTechnique(HLSLTechnique11* technique, HLSLTree* tree, GraphicsDevice* device, Effect *effect);
		~EffectTechnique();

		inline string& GetName() { return m_name; }

		//const
		vector<EffectPass*>& GetPasses();
		friend class Effect;
	private:

		vector<EffectPass*> m_passes;
		string m_name;
	};
}