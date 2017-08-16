#include "stdafx.h"
#include "Shader.h"

#include "Utilities.h"
#include "GraphicsDevice.h"

namespace Cuado {


	Shader::Shader(GraphicsDevice* device, ShaderStage stage, const std::vector<uint8_t>& bytes) :
#if TRIO_DIRECTX
#elif TRIO_OPENGL
		m_shaderHandle(-1),
#endif
		m_device(device),
		m_stage(stage)
	{
#if TRIO_DIRECTX
		m_shaderPtr.m_geometryShader = nullptr;
#elif TRIO_OPENGL
		m_glslCode = string(bytes.begin(), bytes.end());
#endif
		m_iHashKey = 0;
		if (bytes.size() > 0)
		{
			m_codeBytes.resize(bytes.size());
			memcpy(&m_codeBytes[0], &bytes[0], bytes.size() * sizeof(uint8_t));

			m_iHashKey = ComputeHash(&m_codeBytes[0], m_codeBytes.size());
		}

#if TRIO_DIRECTX
		CreateShader();
#endif
	}

#if TRIO_DIRECTX
	Shader::Shader(GraphicsDevice* device, ShaderStage stage, ID3D11VertexShader* shader, const std::vector<uint8_t>& bytes, int hashKey) :
		m_device(device),
		m_stage(stage)
	{
		m_shaderPtr.m_vertexShader = shader;

		m_iHashKey = hashKey;

		if (bytes.size() > 0)
		{
			m_codeBytes.resize(bytes.size());
			memcpy(&m_codeBytes[0], &bytes[0], bytes.size() * sizeof(uint8_t));
		}
	}
#endif

#if TRIO_DIRECTX
	Shader::Shader(GraphicsDevice* device, ShaderStage stage, ID3D11PixelShader* shader) :
		m_device(device),
		m_stage(stage)
	{
		m_shaderPtr.m_pixelShader = shader;

		m_iHashKey = 0;


	}
#endif

	Shader::~Shader()
	{
	}

#if TRIO_OPENGL

	void Shader::GetVertexAttributeLocations(int program)
	{

		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			//m_attributes[i].Location = glGetAttribLocation(program, m_attributes[i].Name.c_str());
		std:stringstream ss;

			glBindAttribLocation(program, i, m_attributes[i].Name.c_str());
			m_attributes[i].Location = i;

			ss << "GetVertexAttributeLocations" << endl;
			ss << "program: " << program << endl;
			ss << "location: " << m_attributes[i].Location << endl;
			ss << "Index: " << m_attributes[i].Index << endl;
			ss << "Usage: " << (int)m_attributes[i].Usage << endl;
			ss << "name: " << m_attributes[i].Name << endl;

			ss << "glsl: " << endl << m_glslCode << endl;

			GraphicsExtensions::checkGLError((char*)ss.str().c_str());
		}
	}

	int Shader::GetAttribLocation(VertexElementUsage usage, int index)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (m_attributes[i].Usage == usage && m_attributes[i].Index == index)
			{
				return m_attributes[i].Location;
			}
		}
		return -1;
	}

	int Shader::GetShaderHandle()
	{
		//std::stringstream ss;
		if (m_shaderHandle != -1)
			return m_shaderHandle;

		GLenum shaderType = GL_VERTEX_SHADER;
		if (m_stage == ShaderStage::Pixel)
			shaderType = GL_FRAGMENT_SHADER;

		//ss << "m_glslCode " << m_glslCode << endl;

		//MessageBoxA(0, ss.str().c_str(), 0, 0);
		m_shaderHandle = glCreateShader(shaderType);
		const GLchar *source = (const GLchar *)m_glslCode.c_str();
		glShaderSource(m_shaderHandle, 1, &source, 0);

		glCompileShader(m_shaderHandle);

		GLint isCompiled = 0;
		glGetShaderiv(m_shaderHandle, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(m_shaderHandle, maxLength, &maxLength, &infoLog[0]);

			//We don't need the shader anymore.
			glDeleteShader(m_shaderHandle);

			//Use the infoLog as you see fit.

			string infoLogStr(infoLog.begin(), infoLog.end());


			//ss << "infoLogStr " << infoLogStr << endl;
			//MessageBoxA(0, ss.str().c_str(), 0, 0);

			//In this simple program, we'll just leave
			m_shaderHandle = -1;
			throw std::exception("shader compilation failed");
		}

		return m_shaderHandle;
	}
#endif

#if TRIO_DIRECTX
	void Shader::CreateShader()
	{
		//m_codeBytes representa el shaderBytecode

		switch (m_stage)
		{
		case Cuado::ShaderStage::Vertex:
			m_device->GetD3DDevice()->CreateVertexShader(&m_codeBytes[0], m_codeBytes.size(), nullptr, &m_shaderPtr.m_vertexShader);

			break;
		case Cuado::ShaderStage::Pixel:
			m_device->GetD3DDevice()->CreatePixelShader(&m_codeBytes[0], m_codeBytes.size(), nullptr, &m_shaderPtr.m_pixelShader);

			break;
		case Cuado::ShaderStage::Geometry:

			break;
		case Cuado::ShaderStage::Compute:
			
			break;
		default:
			break;
		}
	}
#endif
}