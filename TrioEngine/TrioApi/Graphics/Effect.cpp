#include "stdafx.h"
#include "Effect.h"

#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"

namespace Vago
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
		std::string inputFileContent;

		std::ifstream ifs(filename);
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		inputFileContent = buffer.str();
		ifs.close();

		std::vector<TrioFX::HLSLTechnique11*> techniques;
		std::vector<TrioFX::HLSLBuffer*> buffers;
		std::vector<TrioFX::HLSLStruct*> structures;
		std::vector<TrioFX::HLSLDeclaration*> textures;
		std::vector<TrioFX::HLSLDeclaration*> samplers;

		//ID3DBlob* blob = EffectCompiler::CompileShader(filename, "PS", "ps_5_0", nullptr, false);

		TrioMem::Allocator allocator;
		TrioFX::HLSLParser parser(&allocator, filename.c_str(), inputFileContent.data(), inputFileContent.size());
		TrioFX::HLSLTree tree(&allocator);
		if (!parser.Parse(&tree))
		{
			return;
		}

		tree.PopulateEffectCollections(&tree, techniques, buffers, structures, textures, samplers);

		ClearData();

		for (size_t i = 0; i < techniques.size(); i++)
		{
			TrioFX::HLSLTechnique11* tech11 = techniques[i];

			TrioFX::HLSLPass11* pass11 = tech11->passes;
			std::vector<EffectPass*> effectPasses(tech11->numPasses, nullptr);

			for (size_t j = 0; j < tech11->numPasses; j++)
			{
				TrioFX::HLSLPassShader* passShader = pass11->shader;
				std::vector<bool> bufferUsed(buffers.size(), false);

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

						std::vector<size_t> bufferUsedIndex;
						auto itm = m_mShadersByName.find(passShader->options->functionCallName);
						if (itm == m_mShadersByName.end())
						{
							HLSLFunctionVisitor visitor;
							TrioFX::HLSLFunction* functionDef = tree.FindFunction(passShader->options->functionCallName);

							visitor.VisitFunction(functionDef);

							auto it = visitor.m_sParameterVisited.begin();
							for (size_t t = 0; t < visitor.m_sParameterVisited.size(); t++)
							{
								for (size_t w = 0; w < buffers.size(); w++)
								{
									TrioFX::HLSLBuffer* buffer = buffers[w];
									TrioFX::HLSLDeclaration* field = buffer->field;
									bool bFound = false;
									while (field != nullptr)
									{
										if (strcmp(field->name, *it) == 0)
										{
											bFound = true;
											bufferUsed[w] = true;
											break;
										}
										field = (TrioFX::HLSLDeclaration*)field->nextStatement;
									}
									if (bFound) break;
								}
								++it;
							}
							for (size_t t = 0; t < buffers.size(); t++)
							{
								if (bufferUsed[t])
								{
									bufferUsedIndex.push_back(t);
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
							shader = new Shader(m_pDevice, stage, pData);
							shader->m_vBufferIndexes = bufferUsedIndex;

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
							
							m_mShadersByName[passShader->options->functionCallName] = shader;
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
				effectPasses[j] = new EffectPass(pass11->name, vertexShader, pixelShader, m_pDevice, this);

				pass11 = pass11->nextPass;
			}
			EffectTechnique* technique = new EffectTechnique(effectPasses, tech11->name);

			m_Techniques.push_back(technique);
		}

		std::vector<EffectParameter*> parameters;
		for (size_t i = 0; i < buffers.size(); i++)
		{
			TrioFX::HLSLBuffer* buffer = buffers[i];
			TrioFX::HLSLDeclaration* field = buffer->field;
			
			uint32_t bufferSize = 0;
			std::vector<ConstantBufferField> constantFields;
			while (field != nullptr)
			{
				ConstantBufferField cbf(field->name, field->offsetInBytes, field->sizeInBytes, field->typeName);
				constantFields.push_back(cbf);

				bufferSize += field->sizeInBytes;
				field = (TrioFX::HLSLDeclaration*)field->nextStatement;
			}

			ConstantBuffer* nConstantBuffer = new ConstantBuffer(m_pDevice, buffer->name, bufferSize, constantFields);
#if TRIO_OPENGL
			nConstantBuffer->m_uniformBindingIndex = i;
#endif
			m_ConstantBuffers.push_back(nConstantBuffer);
		}

		ConstantBuffer* orphanConstantBuffer = nullptr;
		TrioData::Array<TrioFX::HLSLParser::Variable>& globalVars = parser.GetGlobalVariables();
		for (size_t i = 0; i < globalVars.GetSize(); i++)
		{
			TrioFX::HLSLDeclaration* field = (TrioFX::HLSLDeclaration*)globalVars[i].statement;
			if (field != nullptr)
			{
				if (field->buffer != nullptr)
				{
					ConstantBuffer* constantBuffer = nullptr;
					
					for (size_t j = 0; j < m_ConstantBuffers.size(); j++)
					{
						if (m_ConstantBuffers[j]->m_csName == field->buffer->name) {
							constantBuffer = m_ConstantBuffers[j];
							break;
						}
					}

					for (size_t j = 0; j < constantBuffer->GetFieldsDesc().size(); j++)
					{
						if (constantBuffer->GetFieldsDesc()[j].Name == field->name)
						{
							parameters.push_back(new EffectParameter(constantBuffer->GetFieldsDesc()[j], constantBuffer, this));
							break;
						}
					}					
				}
				else
				{
					if (field->type.baseType >= TrioFX::HLSLBaseType_FirstNumeric && field->type.baseType <= TrioFX::HLSLBaseType_LastNumeric) {
						if (orphanConstantBuffer == nullptr) {
							orphanConstantBuffer = new ConstantBuffer(m_pDevice, "{Orphans}");
						}

						orphanConstantBuffer->GetFieldsDesc().push_back(ConstantBufferField(field->name, field->offsetInBytes, field->sizeInBytes, field->typeName));
					}
				}
			}
		}

		m_Parameters = EffectParameterCollection(parameters);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Effect::HLSLFunctionVisitor::HLSLFunctionVisitor()
	{

	}

	void Effect::HLSLFunctionVisitor::VisitIdentifierExpression(TrioFX::HLSLIdentifierExpression * node)
	{
		if (node->global)
		{
			m_sParameterVisited.insert(node->name);
		}
	}
}