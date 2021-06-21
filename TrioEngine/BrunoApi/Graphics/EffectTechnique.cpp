#include "stdafx.h"
#include "EffectTechnique.h"

#include "EffectPass.h"

namespace BrunoEngine
{
	EffectTechnique::EffectTechnique(BrunoFX::HLSLTechnique11* technique, BrunoFX::HLSLTree& tree, std::vector<BrunoFX::HLSLBuffer*> &buffers, std::vector<BrunoFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect* effect)
	{
		m_name = technique->name;

		m_passes = std::vector<EffectPass *>(technique->numPasses, nullptr);
		BrunoFX::HLSLPass11* ppass = technique->passes;
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