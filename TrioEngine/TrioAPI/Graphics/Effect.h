#pragma once

#include "TrioAPI.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "EffectParameterCollection.h"
#include "EffectTechniqueCollection.h"
#include "ConstantBufferCollection.h"

#include "../../TrioFX/HLSLParser.h"
#include "../../TrioFX/GLSLGenerator.h"
#include "../../TrioFX/Log.h"

using namespace CuadoFX;

namespace Cuado
{
	enum class ShaderLanguageTarget
	{
		HLSL,
		GLSL
	};

	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL Shader;
	
	class TRIOAPI_DLL Effect
	{
	public:
		Effect(GraphicsDevice* device);
		Effect(GraphicsDevice* device, std::vector<uint8_t> compiledBytes);
		Effect(GraphicsDevice* device, std::string filename, ShaderLanguageTarget target = ShaderLanguageTarget::HLSL);
		~Effect();

		inline EffectParameterCollection& GetParameters()		{ return m_Parameters; }
		inline EffectTechniqueCollection& GetTechniques()		{ return m_Techniques; }
		inline ConstantBufferCollection& GetConstantBuffers()	{ return m_ConstantBuffers; }

		void CompileEffect(std::string filename, ShaderLanguageTarget target = ShaderLanguageTarget::HLSL);
	private:

		GraphicsDevice* m_pDevice;
		map<string, Shader*> m_mShadersByName;

		EffectParameterCollection	m_Parameters;
		EffectTechniqueCollection	m_Techniques;
		ConstantBufferCollection	m_ConstantBuffers;

		void ClearData();

		class HLSLFunctionVisitor : public HLSLTreeVisitor
		{
		public:
			HLSLFunctionVisitor();
			void VisitIdentifierExpression(HLSLIdentifierExpression * node);

			set<const char*> m_parameterVisited;
		};
	};
}