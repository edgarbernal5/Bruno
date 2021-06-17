#pragma once

#include "TrioApiRequisites.h"

#include "ShaderLanguageTarget.h"
#include <map>
#include <set>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "EffectParameterCollection.h"
#include "EffectTechniqueCollection.h"
#include "ConstantBufferCollection.h"

#include "FX\HLSLTree.h"

namespace TrioEngine
{
	class GraphicsDevice;
	class Shader;

	class BRUNO_API_EXPORT Effect
	{
	public:
		Effect(GraphicsDevice* device);
		Effect(GraphicsDevice* device, std::vector<uint8_t> &compiledBytes);
		~Effect();

		inline EffectParameterCollection& GetParameters() { return m_parameters; }
		inline EffectTechniqueCollection& GetTechniques() { return m_techniques; }
		inline ConstantBufferCollection& GetConstantBuffers() { return m_constantBuffers; }

		void CompileEffectFromFile(std::string filename, ShaderLanguageTarget target = ShaderLanguageTarget::HLSL);

		friend class EffectPass;
		friend class EffectLoader;
		
	private:
		GraphicsDevice*			m_device;
		std::map<std::string, Shader*>	m_shadersByName;

		EffectParameterCollection	m_parameters;
		EffectTechniqueCollection	m_techniques;
		ConstantBufferCollection	m_constantBuffers;

		void ClearData();
	};
}