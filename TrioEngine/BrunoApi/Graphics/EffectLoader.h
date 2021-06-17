#pragma once

#include "TrioApiRequisites.h"

#include "ShaderLanguageTarget.h"
#include <map>
#include <set>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "..\FX\HLSLTree.h"

namespace BrunoEngine
{
	class Effect;

	class BRUNO_API_EXPORT EffectLoader
	{
	public:
		EffectLoader(Effect* effect);
		~EffectLoader();

		bool LoadFromFile(std::string filename);

	private:
		Effect& m_effect;
	};
}