#include "stdafx.h"
#include "EffectLoader.h"

#include "Effect.h"
#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"

#include <queue>

namespace BrunoEngine
{
	EffectLoader::EffectLoader(Effect* effect) :
		m_effect(*effect)
	{

	}

	EffectLoader::~EffectLoader()
	{

	}

	bool EffectLoader::LoadFromFile(std::string filename)
	{
		std::string inputFileContent;

		std::ifstream ifs(filename);
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		inputFileContent = buffer.str();
		ifs.close();

		//ID3DBlob* blob = EffectCompiler::CompileShader(filename, "PS", "ps_5_0", nullptr, false);

		TrioMem::Allocator allocator;
		TrioFX::HLSLParser parser(&allocator, filename.c_str(), inputFileContent.data(), inputFileContent.size());
		TrioFX::HLSLTree tree(&allocator);
		if (!parser.Parse(&tree))
		{
			return false;
		}
		m_effect.ClearData();

		std::vector<TrioFX::HLSLTechnique11*> techniques;
		std::vector<TrioFX::HLSLBuffer*> buffers;
		std::vector<TrioFX::HLSLStruct*> structures;
		std::vector<TrioFX::HLSLDeclaration*> textures;
		std::vector<TrioFX::HLSLDeclaration*> samplers;

		tree.PopulateEffectCollections(&tree, techniques, buffers, structures, textures, samplers);

		for (size_t i = 0; i < techniques.size(); i++)
		{
			TrioFX::HLSLTechnique11* tech11 = techniques[i];

			EffectTechnique* technique = new EffectTechnique(tech11, tree, buffers, samplers, m_effect.m_device, &m_effect);

			m_effect.m_techniques.push_back(technique);
		}

		for (size_t i = 0; i < buffers.size(); i++)
		{
			TrioFX::HLSLBuffer* buffer = buffers[i];
			TrioFX::HLSLDeclaration* field = buffer->field;

			std::vector<ConstantBufferField> constantFields;
			while (field != nullptr)
			{
				ConstantBufferField cbf(field->name, field->offsetInBytes, field->sizeInBytes, field->typeName);
				constantFields.push_back(cbf);

				field = (TrioFX::HLSLDeclaration*)field->nextStatement;
			}

			ConstantBuffer* nConstantBuffer = new ConstantBuffer(m_effect.m_device, buffer->name, buffer->sizeInBytes, constantFields);
#if BRUNO_OPENGL
			nConstantBuffer->m_uniformBindingIndex = i;
#endif
			m_effect.m_constantBuffers.push_back(nConstantBuffer);
		}

		std::vector<EffectParameter*> parameters;
		ConstantBuffer* orphanConstantBuffer = nullptr;
		TrioData::Array<TrioFX::HLSLParser::Variable>& globalVars = parser.GetGlobalVariables();

		std::queue<EffectParameter*> lazyAppend;
		std::queue<EffectParameter*> lazyAppend2;

		for (size_t i = 0; i < globalVars.GetSize(); i++)
		{
			TrioFX::HLSLDeclaration* field = (TrioFX::HLSLDeclaration*)globalVars[i].statement;
			if (field->buffer != nullptr)
			{
				ConstantBuffer* constantBuffer = nullptr;

				for (size_t j = 0; j < m_effect.m_constantBuffers.size(); j++)
				{
					if (m_effect.m_constantBuffers[j]->m_name == field->buffer->name) {
						constantBuffer = m_effect.m_constantBuffers[j];
						break;
					}
				}

				for (size_t j = 0; j < constantBuffer->GetFieldsDesc().size(); j++)
				{
					if (constantBuffer->GetFieldsDesc()[j].Name == field->name)
					{
						lazyAppend.push(new EffectParameter(constantBuffer->GetFieldsDesc()[j], constantBuffer, &m_effect));
						break;
					}
				}
			}
			else
			{
				if (field->type.baseType >= TrioFX::HLSLBaseType_FirstNumeric && field->type.baseType <= TrioFX::HLSLBaseType_LastNumeric)
				{
					if (orphanConstantBuffer == nullptr)
					{
						orphanConstantBuffer = new ConstantBuffer(m_effect.m_device, "{Orphanage}");
					}
					ConstantBufferField cbf(field->name, field->offsetInBytes, field->sizeInBytes, field->typeName);
					orphanConstantBuffer->GetFieldsDesc().push_back(cbf);

					parameters.push_back(new EffectParameter(cbf, orphanConstantBuffer, &m_effect));
				}
				else
				{
					if (field->type.baseType == TrioFX::HLSLBaseType_Texture2D)
					{
						EffectParameter::STexture sTexture;
						sTexture.m_texture = nullptr;
						sTexture.textureSlot = lazyAppend2.size();

						if (field->registerName != nullptr)
						{
							std::string registerName = field->registerName;
							if (registerName[0] == 't')
							{
								sTexture.textureSlot = std::stoi(registerName.substr(1, registerName.size() - 1));
							}
						}
						lazyAppend2.push(new EffectParameter(sTexture, field->name, &m_effect));
					}
					else if (field->type.baseType == TrioFX::HLSLBaseType_SamplerState)
					{
						EffectParameter::SSamplerState sSamplerState;
						sSamplerState.m_samplerState = nullptr;
						sSamplerState.samplerSlot = lazyAppend2.size();

						if (field->registerName != nullptr)
						{
							std::string registerName = field->registerName;
							if (registerName[0] == 's')
							{
								sSamplerState.samplerSlot = std::stoi(registerName.substr(1, registerName.size() - 1));
							}
						}

						lazyAppend2.push(new EffectParameter(sSamplerState, field->name, &m_effect));
					}
				}
			}
		}
		if (orphanConstantBuffer)
		{
			m_effect.m_constantBuffers.push_back(orphanConstantBuffer);
		}

		while (!lazyAppend2.empty())
		{
			EffectParameter* &q = lazyAppend2.front(); lazyAppend2.pop();
			parameters.push_back(q);
		}

		while (!lazyAppend.empty())
		{
			EffectParameter* &q = lazyAppend.front(); lazyAppend.pop();
			parameters.push_back(q);
		}

		m_effect.m_parameters = EffectParameterCollection(parameters);
		return true;
	}
}