#pragma once

#include "TrioApiRequisites.h"

#include "ShaderStage.h"
#include <vector>

namespace TrioEngine
{
	class GraphicsDevice;
#if TRIO_DIRECTX
	class InputLayoutCache;
#endif

	class TRIO_API_EXPORT Shader
	{
	public:
		Shader(GraphicsDevice* device, ShaderStage stage, const std::vector<uint8_t>& bytes);
		~Shader();

		inline int GetHashKey() { return m_hashKey; }

#if TRIO_DIRECTX
		inline InputLayoutCache* GetInputLayouts() { return m_inputLayoutCache; }
#endif
		friend class Effect;
		friend class EffectLoader;
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
		int					m_hashKey;

		//m_codeBytes representa el shaderBytecode.
		std::vector<uint8_t>	 m_codeBytes;

		GraphicsDevice*			m_device;
		std::vector<std::pair<size_t, size_t>>		m_bufferIndexes;
		std::vector<std::pair<size_t, size_t>>		m_samplerIndexes;

#if TRIO_DIRECTX
		InputLayoutCache*		m_inputLayoutCache;

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

		int GetShaderHandle();
		int m_shaderHandle;

		void GetVertexAttributeLocations(int program);
		int GetAttribLocation(VertexElementUsage usage, int index);
		std::vector<Attribute> m_attributes;
#endif

	};
}