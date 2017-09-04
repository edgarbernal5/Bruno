
#pragma once
#if TRIO_OPENGL

#include "TrioAPI.h"

#include <map>
#include <string>
#include "Shader.h"

namespace Cuado
{
	class TRIOAPI_DLL Effect;
	class TRIOAPI_DLL Shader;

	class TRIOAPI_DLL ShaderProgram
	{
	public:
		ShaderProgram();
		~ShaderProgram();

		GLuint GetUniformLocation(std::string name);

		inline void SetProgram(GLuint program)	{ m_program = program; }
		inline GLuint GetProgram()				{ return m_program; }
	private:
		GLuint m_program;
		std::map<std::string, GLuint> m_uniformLocations;
	};

	class TRIOAPI_DLL ShaderProgramCache
	{
	public:
		ShaderProgramCache();
		~ShaderProgramCache();

		void Clear();
		ShaderProgram* GetProgram(Shader* vertexShader, Shader* pixelShader, Effect* effect);
	private:
		std::map<int, ShaderProgram*> m_programCache;

		ShaderProgram* Link(Shader* vertexShader, Shader* pixelShader, Effect* effect);
	};
}


#endif