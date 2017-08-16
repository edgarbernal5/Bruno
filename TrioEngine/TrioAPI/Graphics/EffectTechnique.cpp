#include "stdafx.h"
#include "EffectTechnique.h"

#include "GraphicsDevice.h"
#include "Effect.h"
#include "EffectPass.h"

namespace Cuado
{

	EffectTechnique::EffectTechnique()
	{

	}

	EffectTechnique::EffectTechnique(HLSLTechnique11* technique, HLSLTree* tree, GraphicsDevice* device, Effect* effect)
	{
		m_name = technique->name;

		m_passes = vector<EffectPass *>(technique->numPasses, nullptr);
		HLSLPass11* ppass = technique->passes;
		for (int i = 0; i < technique->numPasses; i++)
		{
			EffectPass* pass = new EffectPass(ppass, tree, device, effect);
			m_passes[i] = pass;

			ppass = ppass->nextPass;
		}
	}


	EffectTechnique::EffectTechnique(vector<EffectPass*>& passes, const char* name) :
		m_passes(passes), m_name(name)
	{

	}
	
	EffectTechnique::~EffectTechnique()
	{
	}

	vector<EffectPass*>& EffectTechnique::GetPasses()
	{
		return m_passes;
	}
}