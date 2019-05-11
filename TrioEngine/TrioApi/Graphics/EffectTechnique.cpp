#include "stdafx.h"
#include "EffectTechnique.h"

#include "EffectPass.h"

namespace TrioEngine
{
	EffectTechnique::EffectTechnique(TrioFX::HLSLTechnique11* technique, TrioFX::HLSLTree& tree, std::vector<TrioFX::HLSLBuffer*> &buffers, std::vector<TrioFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect* effect)
	{
		m_csName = technique->name;

		m_vPasses = std::vector<EffectPass *>(technique->numPasses, nullptr);
		TrioFX::HLSLPass11* ppass = technique->passes;
		for (int i = 0; i < technique->numPasses; i++)
		{
			m_vPasses[i] = new EffectPass(ppass, tree, buffers, samplers, device, effect);

			ppass = ppass->nextPass;
		}
	}


	EffectTechnique::EffectTechnique(std::vector<EffectPass*>& passes, const char* name) :
		m_vPasses(passes), m_csName(name)
	{

	}

	EffectTechnique::~EffectTechnique()
	{
	}

	std::vector<EffectPass*>& EffectTechnique::GetPasses()
	{
		return m_vPasses;
	}
}