

#include "stdafx.h"
#include "ShaderProgram.h"
#if TRIO_OPENGL
#include "Shader.h"
#include "Effect.h"

namespace Cuado
{

	ShaderProgram::ShaderProgram() : 
		m_program(0)
	{
	}


	ShaderProgram::~ShaderProgram()
	{
	}


	GLuint ShaderProgram::GetUniformLocation(std::string name)
	{
		auto it = m_uniformLocations.find(name);
		if (it != m_uniformLocations.end())
			return it->second;

		GLint location = glGetUniformLocation(m_program, name.c_str());
		if (location >= 0)
			m_uniformLocations[name] = location;
		return location;
	}

	//-------------------------------////---------------------------------------------------//

	ShaderProgramCache::ShaderProgramCache()
	{
	}


	ShaderProgramCache::~ShaderProgramCache()
	{
	}

	void ShaderProgramCache::Clear()
	{
		for (auto& kv : m_programCache)
		{
			if (glIsProgram(kv.second->GetProgram()))
			{
				glDeleteProgram(kv.second->GetProgram());
			}
		}
		m_programCache.clear();
	}

	ShaderProgram* ShaderProgramCache::GetProgram(Shader* vertexShader, Shader* pixelShader, Effect* effect)
	{
		int key = vertexShader->GetHashKey() | pixelShader->GetHashKey();
		auto it = m_programCache.find(key);
		if (it == m_programCache.end())
		{
			return Link(vertexShader, pixelShader, effect);
		}
		return it->second;
	}

	ShaderProgram* ShaderProgramCache::Link(Shader* vertexShader, Shader* pixelShader, Effect* effect)
	{
		//std::stringstream ss;
		glUseProgram(0);
		GLuint programGL = glCreateProgram();
		CHECK_GL_ERROR(glCreateProgram);

		glAttachShader(programGL, vertexShader->GetShaderObject());
		CHECK_GL_ERROR(glAttachShader);
		glAttachShader(programGL, pixelShader->GetShaderObject());
		CHECK_GL_ERROR(glAttachShader);

		//glBindAttribLocation
		vertexShader->GetVertexAttributeLocations(programGL);
		//TO-DO: arreglar esto, pueden ser mas de un bindfragdata. i.e. Multiple render targets.
		glBindFragDataLocation(programGL, 0, "rast_SV_Target");
		CHECK_GL_ERROR(glBindFragDataLocation);

		glLinkProgram(programGL);

		glUseProgram(programGL);
		CHECK_GL_ERROR(glUseProgram);

		//vertexShader->GetVertexAttributeLocations(programGL);
		//pixelShader->ApplySamplerTextureUnits(programGL);


		GLint isLinked = 0;
		glGetProgramiv(programGL, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(programGL, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(programGL, maxLength, &maxLength, &infoLog[0]);

			//Don't leak shaders either.
			glDeleteShader(vertexShader->GetShaderObject());
			glDeleteShader(pixelShader->GetShaderObject());

			//We don't need the programGL anymore.
			glDeleteProgram(programGL);

			//string linkstr(infoLog.begin(), infoLog.end());
			//Use the infoLog as you see fit.

			//ss << "linkstr " << linkstr << endl;
			//MessageBoxA(0, ss.str().c_str(), 0, 0);

			//In this simple programGL, we'll just leave
			throw std::exception("unable to link");
		}

		ShaderProgram* shaderProgram = new ShaderProgram();
		shaderProgram->SetProgram(programGL);
		int key = vertexShader->GetHashKey() | pixelShader->GetHashKey();
		m_programCache[key] = shaderProgram;

		glUseProgram(0);

		effect->m_programID = programGL;
		/*for (auto&buffer : effect->GetConstantBuffers())
		{
		buffer->InitializeUniformData(shaderProgram);
		}*/

		return shaderProgram;
	}
}

#endif