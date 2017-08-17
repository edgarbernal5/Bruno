#include "stdafx.h"
#include "Effect.h"

#include "Shader.h"
#include "EffectCompiler.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "EffectParameter.h"


namespace Cuado
{

	Effect::Effect(GraphicsDevice* device) :
		m_pDevice(device)

#ifdef TRIO_DIRECTX
#elif TRIO_OPENGL
#endif
	{

	}

	Effect::Effect(GraphicsDevice* device, std::vector<uint8_t> compiledBytes)
	{

	}

	Effect::Effect(GraphicsDevice* device, std::string filename, ShaderLanguageTarget target) :
		m_pDevice(device)
#ifdef TRIO_DIRECTX

#endif
	{
		CompileEffect(filename, target);
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

	void Effect::CompileEffect(std::string filename, ShaderLanguageTarget target)
	{
		string inputFileContent;

		ifstream ifs(filename);
		stringstream buffer;
		buffer << ifs.rdbuf();
		inputFileContent = buffer.str();
		ifs.close();

		std::vector<HLSLTechnique11*> techniques;
		std::vector<HLSLBuffer*> buffers;
		std::vector<HLSLStruct*> structures;

		HLSLParser parser(filename.c_str(), inputFileContent.data(), inputFileContent.size());
		HLSLTree tree;
		if (!parser.Parse(&tree))
		{
			return;
		}

		tree.PopulateEffectCollections(&tree, techniques, buffers, structures);

		ClearData();

		for (size_t i = 0; i < techniques.size(); i++)
		{
			HLSLTechnique11* tech11 = techniques[i];

			HLSLPass11* pass11 = tech11->passes;
			std::vector<EffectPass*> effectPasses(tech11->numPasses, nullptr);

			for (size_t j = 0; j < tech11->numPasses; j++)
			{
				HLSLPassShader* passShader = pass11->shader;
				std::vector<bool> bufferUsed(buffers.size(), false);

				Shader* pixelShader = nullptr;
				Shader* vertexShader = nullptr;
				Shader* geometryShader = nullptr;

				for (size_t k = 0; k < pass11->numPassShader; k++)
				{
					if (strcmp(passShader->name, "VertexShader") == 0 || strcmp(passShader->name, "PixelShader") == 0) {
						vector<size_t> bufferUsedIndex;
						auto itm = m_mShadersByName.find(passShader->options->functionCallName);
						if (itm == m_mShadersByName.end())
						{
							HLSLFunctionVisitor visitor;
							HLSLFunction* functionDef = tree.FindFunction(passShader->options->functionCallName);

							visitor.VisitFunction(functionDef);

							auto it = visitor.m_parameterVisited.begin();
							for (size_t t = 0; t < visitor.m_parameterVisited.size(); t++)
							{
								for (size_t w = 0; w < buffers.size(); w++)
								{
									HLSLBuffer* buffer = buffers[w];
									HLSLDeclaration* field = buffer->field;
									bool bFound = false;
									while (field != nullptr)
									{
										if (strcmp(field->name, *it) == 0)
										{
											bFound = true;
											bufferUsed[w] = true;
											break;
										}
										field = (HLSLDeclaration*)field->nextStatement;
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

							ShaderStage stage = ShaderStage::Vertex;
							if (strcmp(passShader->name, "PixelShader") == 0)
								stage = ShaderStage::Pixel;

							string intermediario;
							EffectCompiler::GenerateShaderFile(&tree, stage, passShader->options->functionCallName, intermediario);

#if TRIO_DIRECTX
							string temporalFilename = string(pass11->fileName) + "tmp";

							std::ofstream out(temporalFilename);
							out << intermediario;
							out.close();

							ID3DBlob* blob = EffectCompiler::CompileShader(temporalFilename, passShader->options->functionCallName, passShader->options->profile, nullptr, false);

							vector<uint8_t> pData = vector<uint8_t>(blob->GetBufferSize());

							memcpy(&pData[0], blob->GetBufferPointer(), blob->GetBufferSize());
#else
							vector<uint8_t> pData(intermediario.begin(), intermediario.end());

#endif
							Shader* shader = new Shader(m_pDevice, stage, pData);
							shader->m_bufferIndexes = bufferUsedIndex;

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
							switch (stage)
							{
							case Cuado::ShaderStage::Vertex:
								vertexShader = shader;
								break;
							case Cuado::ShaderStage::Pixel:
								pixelShader = shader;
								break;
							case Cuado::ShaderStage::Geometry:
								break;
							case Cuado::ShaderStage::Compute:
								break;
							default:
								break;
							}
							m_mShadersByName[passShader->options->functionCallName] = shader;
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

		size_t totalBufferFields = 0;
		vector<EffectParameter*> parameters;
		for (size_t i = 0; i < buffers.size(); i++)
		{
			HLSLBuffer* buffer = buffers[i];
			HLSLDeclaration* field = buffer->field;

			uint32_t bufferSize = 0;
			vector<ConstantBufferField> constantFields;
			while (field != nullptr)
			{
				uint32_t offset = bufferSize;
				uint32_t fieldSize = 0;
				string paramterName = field->name;
				string baseTypeName = "";

				switch (field->type.baseType)
				{
				case HLSLBaseType_Int:
					fieldSize = 1 * 4;
					baseTypeName = "Int";
					break;
				case HLSLBaseType_Float:
					fieldSize = 1 * 4;
					baseTypeName = "Float";
					break;

				case HLSLBaseType_Int2:
					fieldSize = 2 * 4;
					baseTypeName = "Int2";
					break;
				case HLSLBaseType_Float2:
					baseTypeName = "Float2";
					fieldSize = 2 * 4;
					break;

				case HLSLBaseType_Int3:
					baseTypeName = "Int3";
					fieldSize = 3 * 4;
					break;
				case HLSLBaseType_Float3:
					baseTypeName = "Float3";
					fieldSize = 3 * 4;
					break;

				case HLSLBaseType_Int4:
					baseTypeName = "Int4";
					fieldSize = 4 * 4;
					break;
				case HLSLBaseType_Float4:
					baseTypeName = "Float4";
					fieldSize = 4 * 4;
					break;

				case HLSLBaseType_Float3x3:
					baseTypeName = "Float3x3";
					fieldSize = 3 * 3 * 4;
					break;

				case HLSLBaseType_Float4x4:
					baseTypeName = "Float4x4";
					fieldSize = 4 * 4 * 4;
					break;


				case HLSLBaseType_Half:
					baseTypeName = "Half";
					fieldSize = 1 * 2;
					break;

				case HLSLBaseType_Half2:
					baseTypeName = "Half2";
					fieldSize = 2 * 2;
					break;

				case HLSLBaseType_Half3:
					baseTypeName = "Half3";
					fieldSize = 3 * 2;
					break;

				case HLSLBaseType_Half4:
					baseTypeName = "Half4";
					fieldSize = 4 * 2;
					break;

				case HLSLBaseType_Half4x4:
					baseTypeName = "Half4x4";
					fieldSize = 4 * 4 * 2;
					break;

				case HLSLBaseType_Half3x3:
					baseTypeName = "Half3x3";
					fieldSize = 3 * 3 * 2;
					break;
				}
				bufferSize += fieldSize;
				ConstantBufferField cbf(paramterName, offset, fieldSize, baseTypeName);
				constantFields.push_back(cbf);

				field = (HLSLDeclaration*)field->nextStatement;
			}
			ConstantBuffer* nConstantBuffer = new ConstantBuffer(m_pDevice, buffer->name, bufferSize, constantFields);
#if TRIO_OPENGL
			nConstantBuffer->m_uniformBindingIndex = i;
#endif
			m_ConstantBuffers.push_back(nConstantBuffer);

			for (size_t j = 0; j < constantFields.size(); j++)
			{
				parameters.push_back(new EffectParameter(constantFields[j], nConstantBuffer, this));
			}
			totalBufferFields += constantFields.size();
		}
		m_Parameters = EffectParameterCollection(parameters);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Effect::HLSLFunctionVisitor::HLSLFunctionVisitor()
	{

	}

	void Effect::HLSLFunctionVisitor::VisitIdentifierExpression(HLSLIdentifierExpression * node)
	{
		if (node->global)
		{
			m_parameterVisited.insert(node->name);
		}
	}
}