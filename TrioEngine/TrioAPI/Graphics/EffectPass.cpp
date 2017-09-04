#include "stdafx.h"
#include "EffectPass.h"

#include "Effect.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Cuado
{
	EffectPass::EffectPass() //:m_Effect(nullptr), m_Device(nullptr), m_vertexShader(nullptr), m_pixelShader(nullptr), m_AssignVS(false), m_AssignPS(false)
	{
	}

	EffectPass::EffectPass(HLSLPass11* pass, HLSLTree* tree, GraphicsDevice* device, Effect* effect)
	{
	}

	EffectPass::EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect) :
		m_vertexShader(vertexShader), m_pixelShader(pixelShader), m_Effect(effect), m_pDevice(device), m_Name(name)
	{

	}

	void EffectPass::Apply() 
	{
		m_pDevice->m_effectPass = this;

#if defined(TRIO_OPENGL) || defined(TRIO_DIRECTX)
		if (m_vertexShader != nullptr)
		{
			m_pDevice->SetVertexShader(m_vertexShader);
			for (size_t i = 0; i < m_vertexShader->m_bufferIndexes.size(); i++)
			{
				ConstantBuffer* constBuffer = m_Effect->m_ConstantBuffers[m_vertexShader->m_bufferIndexes[i]];
				m_pDevice->SetConstantBuffer(ShaderStage::Vertex, i, constBuffer);
			}
		}

		if (m_pixelShader != nullptr)
		{
			m_pDevice->SetPixelShader(m_pixelShader);
			for (size_t i = 0; i < m_pixelShader->m_bufferIndexes.size(); i++)
			{
				ConstantBuffer* constBuffer = m_Effect->m_ConstantBuffers[m_pixelShader->m_bufferIndexes[i]];
				m_pDevice->SetConstantBuffer(ShaderStage::Pixel, i, constBuffer);
			}
		}
#endif
	}
}