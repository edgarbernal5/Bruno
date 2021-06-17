#include "stdafx.h"
#include "EffectPass.h"

#include "Effect.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "EffectCompiler.h"

#include "GraphicsDevice.h"

namespace TrioEngine
{
	EffectPass::EffectPass(TrioFX::HLSLPass11* pass11, TrioFX::HLSLTree& tree, std::vector<TrioFX::HLSLBuffer*> &buffers, std::vector<TrioFX::HLSLDeclaration*> &samplers, GraphicsDevice* device, Effect* effect) :
		m_vertexShader(nullptr), m_pixelShader(nullptr), m_effect(effect), m_device(device)
	{
		TrioFX::HLSLPassShader* passShader = pass11->shader;
		m_name = pass11->name;

		std::vector<bool> buffersUsed(buffers.size(), false);
		std::vector<bool> samplersUsed(samplers.size(), false);

		Shader* pixelShader = nullptr;
		Shader* vertexShader = nullptr;
		Shader* geometryShader = nullptr;

		for (size_t k = 0; k < pass11->numPassShader; k++, passShader = passShader->nextShader)
		{
			if (strcmp(passShader->name, "VertexShader") == 0 || strcmp(passShader->name, "PixelShader") == 0)
			{
				Shader* shader = nullptr;
				ShaderStage stage = ShaderStage::Vertex;
				if (strcmp(passShader->name, "PixelShader") == 0)
					stage = ShaderStage::Pixel;

				std::vector<std::pair<size_t, size_t>> buffersUsedIndexes;
				std::vector<std::pair<size_t, size_t>> samplersUsedIndexes;

				auto itm = m_effect->m_shadersByName.find(passShader->options->functionCallName);
				if (itm == m_effect->m_shadersByName.end())
				{
					TrioFX::HLSLFunctionVisitor visitor;
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
								TrioFX::HLSLDeclaration* sampler = samplers[w];
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
									cbBindingIndex = std::stoi(registerName.substr(2, registerName.size() - 2));
								}
							}

							buffersUsedIndexes.push_back(std::make_pair(t, cbBindingIndex));
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

							samplersUsedIndexes.push_back(std::make_pair(t, cbBindingIndex));
						}
					}

					std::string intermediario;
					EffectCompiler::GenerateShaderFile(&tree, stage, passShader->options->functionCallName, intermediario);

#if BRUNO_DIRECTX
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
					shader = new Shader(m_device, stage, pData);
					shader->m_bufferIndexes = buffersUsedIndexes;
					shader->m_samplerIndexes = samplersUsedIndexes;

#if BRUNO_OPENGL
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

					m_effect->m_shadersByName[passShader->options->functionCallName] = shader;
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
		}

		m_vertexShader = vertexShader;
		m_pixelShader = pixelShader;
	}

	EffectPass::EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect) :
		m_vertexShader(vertexShader), m_pixelShader(pixelShader), m_effect(effect), m_device(device), m_name(name)
	{

	}

	void EffectPass::Apply()
	{
#if defined(BRUNO_OPENGL) || defined(BRUNO_DIRECTX)
		if (m_vertexShader)
		{
			m_device->SetVertexShader(m_vertexShader);
			for (size_t i = 0; i < m_vertexShader->m_bufferIndexes.size(); i++)
			{
				auto pair = m_vertexShader->m_bufferIndexes[i];
				ConstantBuffer* constBuffer = m_effect->m_constantBuffers[pair.first];
				m_device->SetConstantBuffer(ShaderStage::Vertex, i, constBuffer);
			}
		}

		if (m_pixelShader)
		{
			m_device->SetPixelShader(m_pixelShader);
			for (size_t i = 0; i < m_pixelShader->m_bufferIndexes.size(); i++)
			{
				auto pair = m_pixelShader->m_bufferIndexes[i];
				ConstantBuffer* constBuffer = m_effect->m_constantBuffers[pair.first];
				m_device->SetConstantBuffer(ShaderStage::Pixel, i, constBuffer);
			}
		}
#endif
	}
}