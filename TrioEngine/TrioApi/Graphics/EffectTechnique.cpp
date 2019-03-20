#include "stdafx.h"
#include "EffectTechnique.h"

#include "EffectPass.h"

namespace Vago
{
	EffectTechnique::EffectTechnique()
	{

	}

	EffectTechnique::EffectTechnique(TrioFX::HLSLTechnique11* technique, TrioFX::HLSLTree* tree, GraphicsDevice* device, Effect* effect)
	{
		m_csName = technique->name;

		m_vPasses = std::vector<EffectPass *>(technique->numPasses, nullptr);
		TrioFX::HLSLPass11* ppass = technique->passes;
		for (int i = 0; i < technique->numPasses; i++)
		{
			EffectPass* pass = new EffectPass(ppass, tree, device, effect);
			m_vPasses[i] = pass;

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