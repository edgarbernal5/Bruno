#include "stdafx.h"
#include "EffectTechnique.h"

#include "EffectPass.h"

namespace BrunoEngine
{
	EffectTechnique::EffectTechnique(TrioFX::HLSLTechnique11* technique, TrioFX::HLSLTree& tree, std::vector<TrioFX::HLSLBuffer*> &buffers, std::vector<TrioFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect* effect)
	{
		m_name = technique->name;

		m_passes = std::vector<EffectPass *>(technique->numPasses, nullptr);
		TrioFX::HLSLPass11* ppass = technique->passes;
		for (int i = 0; i < technique->numPasses; i++)
		{
			m_passes[i] = new EffectPass(ppass, tree, buffers, samplers, device, effect);

			ppass = ppass->nextPass;
		}
	}


	EffectTechnique::EffectTechnique(std::vector<EffectPass*>& passes, const char* name) :
		m_passes(passes), m_name(name)
	{

	}

	EffectTechnique::~EffectTechnique()
	{
	}

	std::vector<EffectPass*>& EffectTechnique::GetPasses()
	{
		return m_passes;
	}
}