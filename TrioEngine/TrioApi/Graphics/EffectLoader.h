#pragma once

#include "TrioApiDecl.h"

#include "ShaderLanguageTarget.h"
#include <map>
#include <set>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "..\FX\HLSLTree.h"

namespace TrioEngine
{
	class Effect;

	class TRIOAPI_DECL EffectLoader
	{
	public:
		EffectLoader(Effect* effect);
		~EffectLoader();

		bool LoadEffect(std::string filename);

	private:
		Effect& m_Effect;
	};
}