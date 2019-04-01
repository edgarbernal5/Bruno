#include "stdafx.h"
#include "EffectLoader.h"

#include "Effect.h"
#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"

#include <queue>

namespace Vago
{
	EffectLoader::EffectLoader(Effect* effect) :
		m_Effect(*effect)
	{

	}

	EffectLoader::~EffectLoader()
	{

	}

	bool EffectLoader::LoadEffect(std::string filename)
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
		m_Effect.ClearData();

		std::vector<TrioFX::HLSLTechnique11*> techniques;
		std::vector<TrioFX::HLSLBuffer*> buffers;
		std::vector<TrioFX::HLSLStruct*> structures;
		std::vector<TrioFX::HLSLDeclaration*> textures;
		std::vector<TrioFX::HLSLDeclaration*> samplers;

		tree.PopulateEffectCollections(&tree, techniques, buffers, structures, textures, samplers);

		for (size_t i = 0; i < techniques.size(); i++)
		{
			TrioFX::HLSLTechnique11* tech11 = techniques[i];

			TrioFX::HLSLPass11* pass11 = tech11->passes;
			std::vector<EffectPass*> effectPasses(tech11->numPasses, nullptr);

			for (size_t j = 0; j < tech11->numPasses; j++)
			{
				TrioFX::HLSLPassShader* passShader = pass11->shader;
				std::vector<bool> buffersUsed(buffers.size(), false);
				std::vector<bool> samplersUsed(samplers.size(), false);

				Shader* pixelShader = nullptr;
				Shader* vertexShader = nullptr;
				Shader* geometryShader = nullptr;

				for (size_t k = 0; k < pass11->numPassShader; k++)
				{
					if (strcmp(passShader->name, "VertexShader") == 0 || strcmp(passShader->name, "PixelShader") == 0)
					{
						Shader* shader = nullptr;
						ShaderStage stage = ShaderStage::Vertex;
						if (strcmp(passShader->name, "PixelShader") == 0)
							stage = ShaderStage::Pixel;

						std::vector<std::pair<size_t, size_t>> buffersUsedIndexes;
						std::vector<std::pair<size_t, size_t>> samplersUsedIndexes;

						auto itm = m_Effect.m_mShadersByName.find(passShader->options->functionCallName);
						if (itm == m_Effect.m_mShadersByName.end())
						{
							HLSLFunctionVisitor visitor;
							TrioFX::HLSLFunction* functionDef = tree.FindFunction(passShader->options->functionCallName);

							visitor.VisitFunction(functionDef);

							auto it = visitor.m_sParameterVisited.begin();
							for (size_t t = 0; t < visitor.m_sParameterVisited.size(); t++)
							{
								bool bFound = false;
								for (size_t w = 0; w < buffers.size(); w++)
								{
									TrioFX::HLSLBuffer* buffer = buffers[w];
									TrioFX::HLSLDeclaration* field = buffer->field;

									while (field != nullptr)
									{
										if (strcmp(field->name, *it) == 0)
										{
											bFound = true;
											buffersUsed[w] = true;
											break;
										}
										field = (TrioFX::HLSLDeclaration*)field->nextStatement;
									}
									if (bFound) break;
								}

								if (!bFound)
								{
									for (size_t w = 0; w < samplers.size(); w++)
									{
										TrioFX::HLSLDeclaration* sampler = samplers[i];
										if (strcmp(sampler->name, *it) == 0)
										{
											bFound = true;
											samplersUsed[w] = true;
											break;
										}
									}
								}
								++it;
							}
							for (size_t t = 0; t < buffers.size(); t++)
							{
								if (buffersUsed[t])
								{
									size_t cbBindingIndex = t;
									TrioFX::HLSLBuffer* buffer = buffers[t];
									if (buffer->registerName != nullptr)
									{
										std::string registerName = buffer->registerName;
										if (registerName.size() > 2 && registerName[0] == 'c' && registerName[1] == 'b')
										{
											cbBindingIndex = std::stoi(registerName.substr(1, registerName.size() - 1));
										}
									}

									buffersUsedIndexes.push_back(make_pair(t, cbBindingIndex));
								}
							}

							for (size_t t = 0; t < samplers.size(); t++)
							{
								if (samplersUsed[t])
								{
									size_t cbBindingIndex = t;
									TrioFX::HLSLDeclaration* sampler = samplers[t];
									if (sampler->registerName != nullptr)
									{
										std::string registerName = sampler->registerName;
										if (registerName[0] == 's')
										{
											cbBindingIndex = std::stoi(registerName.substr(1, registerName.size() - 1));
										}
									}

									samplersUsedIndexes.push_back(make_pair(t, cbBindingIndex));
								}
							}

							std::string intermediario;
							EffectCompiler::GenerateShaderFile(&tree, stage, passShader->options->functionCallName, intermediario);

#if TRIO_DIRECTX
							std::string temporalFilename = string(pass11->fileName) + "tmp";

							std::ofstream out(temporalFilename);
							out << intermediario;
							out.close();

							ID3DBlob* blob = EffectCompiler::CompileShader(temporalFilename, passShader->options->functionCallName, passShader->options->profile, nullptr, false);

							std::vector<uint8_t> pData = std::vector<uint8_t>(blob->GetBufferSize());

							memcpy(pData.data(), blob->GetBufferPointer(), blob->GetBufferSize());
#else
							vector<uint8_t> pData(intermediario.begin(), intermediario.end());

#endif
							shader = new Shader(m_Effect.m_pDevice, stage, pData);
							shader->m_vBufferIndexes = buffersUsedIndexes;
							shader->m_vSamplerIndexes = samplersUsedIndexes;

#if TRIO_OPENGL
							if (stage == ShaderStage::Vertex)
							{
								vector<Attribute> attributes;
								HLSLArgument* argument = functionDef->argument;
								while (argument != nullptr) {
									if (argument->type.baseType == HLSLBaseType_UserDefined)
									{
										HLSLStruct* structArgument = FindStructByName(structures, argument->type.typeName);
										HLSLStructField* structField = structArgument->field;
										while (structField != nullptr) {
											string nameBaase = VertexChannelNames::DecodeBaseName(structField->semantic);
											int indexField = VertexChannelNames::DecodeUsageIndex(structField->semantic);
											VertexElementUsage usage = VertexElementUsage::Position;
											if (strcmp(nameBaase.c_str(), "POSITION") == 0)
												usage = VertexElementUsage::Position;
											else if (strcmp(nameBaase.c_str(), "COLOR") == 0)
												usage = VertexElementUsage::Color;
											else if (strcmp(nameBaase.c_str(), "TEXCOORD") == 0)
												usage = VertexElementUsage::TextureCoordinate;
											else if (strcmp(nameBaase.c_str(), "NORMAL") == 0)
												usage = VertexElementUsage::Normal;

											attributes.push_back(Attribute(usage, indexField, structField->semantic, -1));
											structField = structField->nextField;

										}
									}
									argument = argument->nextArgument;
								}
								shader->m_attributes = attributes;
							}
#endif

							m_Effect.m_mShadersByName[passShader->options->functionCallName] = shader;
						}
						else
						{
							shader = itm->second;
						}

						switch (stage)
						{
						case ShaderStage::Vertex:
							vertexShader = shader;
							break;
						case ShaderStage::Pixel:
							pixelShader = shader;
							break;
						case ShaderStage::Geometry:
							break;
						case ShaderStage::Compute:
							break;
						default:
							break;
						}
					}
					passShader = passShader->nextShader;
				}
				effectPasses[j] = new EffectPass(pass11->name, vertexShader, pixelShader, m_Effect.m_pDevice, &m_Effect);

				pass11 = pass11->nextPass;
			}
			EffectTechnique* technique = new EffectTechnique(effectPasses, tech11->name);

			m_Effect.m_Techniques.push_back(technique);
		}

		std::vector<EffectParameter*> parameters;
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

			ConstantBuffer* nConstantBuffer = new ConstantBuffer(m_Effect.m_pDevice, buffer->name, buffer->sizeInBytes, constantFields);
#if TRIO_OPENGL
			nConstantBuffer->m_uniformBindingIndex = i;
#endif
			m_Effect.m_ConstantBuffers.push_back(nConstantBuffer);
		}

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

				for (size_t j = 0; j < m_Effect.m_ConstantBuffers.size(); j++)
				{
					if (m_Effect.m_ConstantBuffers[j]->m_csName == field->buffer->name) {
						constantBuffer = m_Effect.m_ConstantBuffers[j];
						break;
					}
				}

				for (size_t j = 0; j < constantBuffer->GetFieldsDesc().size(); j++)
				{
					if (constantBuffer->GetFieldsDesc()[j].Name == field->name)
					{
						lazyAppend.push(new EffectParameter(constantBuffer->GetFieldsDesc()[j], constantBuffer, &m_Effect));
						break;
					}
				}
			}
			else
			{
				if (field->type.baseType >= TrioFX::HLSLBaseType_FirstNumeric && field->type.baseType <= TrioFX::HLSLBaseType_LastNumeric) {
					if (orphanConstantBuffer == nullptr)
					{
						orphanConstantBuffer = new ConstantBuffer(m_Effect.m_pDevice, "{Orphanage}");
					}
					ConstantBufferField cbf(field->name, field->offsetInBytes, field->sizeInBytes, field->typeName);
					orphanConstantBuffer->GetFieldsDesc().push_back(cbf);

					parameters.push_back(new EffectParameter(cbf, orphanConstantBuffer, &m_Effect));
				}
				else
				{
					if (field->type.baseType == TrioFX::HLSLBaseType_Texture2D)
					{
						EffectParameter::STexture sTexture;
						sTexture.m_pTexture = nullptr;
						sTexture.textureSlot = lazyAppend2.size();

						if (field->registerName != nullptr)
						{
							std::string registerName = field->registerName;
							if (registerName[0] == 't')
							{
								sTexture.textureSlot = std::stoi(registerName.substr(1, registerName.size() - 1));
							}
						}
						lazyAppend2.push(new EffectParameter(sTexture, field->name, &m_Effect));
					}
					else if (field->type.baseType == TrioFX::HLSLBaseType_SamplerState)
					{
						EffectParameter::SSamplerState sSamplerState;
						sSamplerState.m_pSamplerState = nullptr;
						sSamplerState.samplerSlot = lazyAppend2.size();

						if (field->registerName != nullptr)
						{
							std::string registerName = field->registerName;
							if (registerName[0] == 's')
							{
								sSamplerState.samplerSlot = std::stoi(registerName.substr(1, registerName.size() - 1));
							}
						}

						lazyAppend2.push(new EffectParameter(sSamplerState, field->name, &m_Effect));
					}
				}
			}
		}
		if (orphanConstantBuffer)
		{
			m_Effect.m_ConstantBuffers.push_back(orphanConstantBuffer);
		}

		while (!lazyAppend.empty()) {
			EffectParameter* &q = lazyAppend.front(); lazyAppend.pop();
			parameters.push_back(q);
		}

		while (!lazyAppend2.empty()) {
			EffectParameter* &q = lazyAppend2.front(); lazyAppend2.pop();
			parameters.push_back(q);
		}

		m_Effect.m_Parameters = EffectParameterCollection(parameters);
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	EffectLoader::HLSLFunctionVisitor::HLSLFunctionVisitor()
	{

	}

	void EffectLoader::HLSLFunctionVisitor::VisitIdentifierExpression(TrioFX::HLSLIdentifierExpression * node)
	{
		if (node->global)
		{
			m_sParameterVisited.insert(node->name);
		}
	}
}