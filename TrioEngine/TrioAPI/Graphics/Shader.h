#ifndef __SHADER_H__
#define __SHADER_H__

#include "TrioAPI.h"

#include "StatesEnums.h"
#include <vector>
#include <string>

#include "VertexElement.h"

#if TRIO_OPENGL

#include "GL/glew.h"
#include "GL/wglew.h"

#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;
#if TRIO_DIRECTX
	class TRIOAPI_DLL InputLayoutCache;
#endif

#if TRIO_OPENGL
	struct Attribute
	{
		VertexElementUsage Usage;
		int Index;
		std::string Name;
		GLint Location;
		Attribute() {}
		Attribute(VertexElementUsage usage, int index, std::string name, int location) : Usage(usage), Index(index), Name(name), Location(location) {}
	};
#endif

	class TRIOAPI_DLL Shader
	{
	public:
		Shader(GraphicsDevice* device, ShaderStage stage, const std::vector<uint8_t>& bytes);
		~Shader();

		inline int GetHashKey() { return m_iHashKey; }
#if TRIO_DIRECTX
		inline InputLayoutCache* GetInputLayouts() { return m_pInputLayoutCache; }
#endif

		friend class Effect;
		friend class EffectPass;
		friend class GraphicsDevice;
		friend class ShaderProgramCache;
		friend class VertexDeclaration;

	private:
#if TRIO_DIRECTX
		Shader(GraphicsDevice* device, ShaderStage stage, ID3D11VertexShader* shader, const std::vector<uint8_t>& bytes, int hashKey);
		Shader(GraphicsDevice* device, ShaderStage stage, ID3D11PixelShader* shader);
#endif
		ShaderStage			m_stage;
		int					m_iHashKey;

		//m_codeBytes representa el shaderBytecode.
		std::vector<uint8_t>	 m_codeBytes;

		GraphicsDevice*			m_device;
		std::vector<size_t>		m_bufferIndexes;

#if TRIO_DIRECTX
		InputLayoutCache*		m_pInputLayoutCache;

		union InternalShader
		{
			ID3D11VertexShader* m_vertexShader;
			ID3D11PixelShader* m_pixelShader;
			ID3D11GeometryShader* m_geometryShader;
		};

		InternalShader		m_shaderPtr;

		void CreateShader();

#elif TRIO_OPENGL
		std::string				m_glslCode;

		int GetShaderObject();
		int m_iShaderObject;

		void GetVertexAttributeLocations(int program);
		int GetAttribLocation(VertexElementUsage usage, int index);
		std::vector<Attribute> m_attributes;

#endif
	};
}



#endif