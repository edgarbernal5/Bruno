#pragma once

#include "TrioApiDecl.h"

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

#include "..\FX\HLSLTree.h"

namespace Vago
{
	class GraphicsDevice;
	class Shader;

	class TRIOAPI_DECL Effect
	{
	public:
		Effect(GraphicsDevice* device);
		Effect(GraphicsDevice* device, std::vector<uint8_t> compiledBytes);
		Effect(GraphicsDevice* device, std::string filename, ShaderLanguageTarget target = ShaderLanguageTarget::HLSL);
		~Effect();

		inline EffectParameterCollection& GetParameters() { return m_Parameters; }
		inline EffectTechniqueCollection& GetTechniques() { return m_Techniques; }
		inline ConstantBufferCollection& GetConstantBuffers() { return m_ConstantBuffers; }

		void CompileEffect(std::string filename, ShaderLanguageTarget target = ShaderLanguageTarget::HLSL);

		friend class EffectPass;
	private:

		GraphicsDevice*			m_pDevice;
		std::map<std::string, Shader*>	m_mShadersByName;

		EffectParameterCollection	m_Parameters;
		EffectTechniqueCollection	m_Techniques;
		ConstantBufferCollection	m_ConstantBuffers;

		void ClearData();

		class HLSLFunctionVisitor : public TrioFX::HLSLTreeVisitor
		{
		public:
			HLSLFunctionVisitor();
			void VisitIdentifierExpression(TrioFX::HLSLIdentifierExpression * node);

			std::set<const char*> m_sParameterVisited;
		};
	};
}