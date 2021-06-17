#include "stdafx.h"
#include "Effect.h"

#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"

#include "EffectLoader.h"

namespace BrunoEngine
{
	Effect::Effect(GraphicsDevice* device) :
		m_device(device)

#ifdef BRUNO_DIRECTX
#elif BRUNO_OPENGL
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
		auto it = m_shadersByName.begin();
		while (it != m_shadersByName.end())
		{
			Shader* shader = it->second;
			if (shader != nullptr) delete shader;

			++it;
		}

		for (size_t i = 0; i < m_techniques.size(); i++)
		{
			EffectTechnique* technique = m_techniques[i];
			if (technique != nullptr) delete technique;
		}

		for (size_t i = 0; i < m_constantBuffers.size(); i++)
		{
			ConstantBuffer* buffer = m_constantBuffers[i];
			if (buffer != nullptr) delete buffer;
		}

		for (size_t i = 0; i < m_parameters.size(); i++)
		{
			EffectParameter* parameter = m_parameters[i];
			if (parameter != nullptr) delete parameter;
		}

		m_shadersByName.clear();
		m_techniques.clear();
		m_constantBuffers.clear();
		m_parameters.clear();
	}

	void Effect::CompileEffectFromFile(std::string filename, ShaderLanguageTarget target)
	{
		EffectLoader loader(this);

		loader.LoadFromFile(filename);
	}
}