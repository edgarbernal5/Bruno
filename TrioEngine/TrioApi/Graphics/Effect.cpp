#include "stdafx.h"
#include "Effect.h"

#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"

#include "EffectLoader.h"

namespace TrioEngine
{
	Effect::Effect(GraphicsDevice* device) :
		m_pDevice(device)

#ifdef TRIO_DIRECTX
#elif TRIO_OPENGL
#endif
	{

	}

	Effect::Effect(GraphicsDevice* device, std::vector<uint8_t> &compiledBytes)
	{

	}

	Effect::~Effect()
	{
		ClearData();
	}

	void Effect::ClearData()
	{
		auto it = m_mShadersByName.begin();
		while (it != m_mShadersByName.end())
		{
			Shader* shader = it->second;
			if (shader != nullptr) delete shader;

			++it;
		}

		for (size_t i = 0; i < m_Techniques.size(); i++)
		{
			EffectTechnique* technique = m_Techniques[i];
			if (technique != nullptr) delete technique;
		}

		for (size_t i = 0; i < m_ConstantBuffers.size(); i++)
		{
			ConstantBuffer* buffer = m_ConstantBuffers[i];
			if (buffer != nullptr) delete buffer;
		}

		for (size_t i = 0; i < m_Parameters.size(); i++)
		{
			EffectParameter* parameter = m_Parameters[i];
			if (parameter != nullptr) delete parameter;
		}

		m_mShadersByName.clear();
		m_Techniques.clear();
		m_ConstantBuffers.clear();
		m_Parameters.clear();
	}

	void Effect::CompileEffectFromFile(std::string filename, ShaderLanguageTarget target)
	{
		EffectLoader loader(this);

		loader.LoadEffect(filename);
	}
}