#include "stdafx.h"
#include "EffectPass.h"


namespace Cuado
{
	EffectPass::EffectPass() //:m_Effect(nullptr), m_Device(nullptr), m_vertexShader(nullptr), m_pixelShader(nullptr), m_AssignVS(false), m_AssignPS(false)
	{
	}

	EffectPass::EffectPass(HLSLPass11* pass, HLSLTree* tree, GraphicsDevice* device, Effect* effect)
	{
	}

	EffectPass::EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect) :
		m_vertexShader(vertexShader), m_pixelShader(pixelShader), m_Effect(effect), m_pDevice(device), m_Name(name)
	{

	}
}