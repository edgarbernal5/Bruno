#include "stdafx.h"
#include "EffectPass.h"

#include "Effect.h"
#include "Shader.h"
#include "ConstantBuffer.h"

#include "GraphicsDevice.h"

namespace Vago
{
	EffectPass::EffectPass(TrioFX::HLSLPass11* pass, TrioFX::HLSLTree* tree, GraphicsDevice* device, Effect* effect) :
		m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pEffect(effect), m_pDevice(device)
	{
	}

	EffectPass::EffectPass(const char* name, Shader* vertexShader, Shader* pixelShader, GraphicsDevice* device, Effect* effect) :
		m_pVertexShader(vertexShader), m_pPixelShader(pixelShader), m_pEffect(effect), m_pDevice(device), m_csName(name)
	{

	}

	void EffectPass::Apply()
	{
#if defined(TRIO_OPENGL) || defined(TRIO_DIRECTX)
		if (m_pVertexShader)
		{
			m_pDevice->SetVertexShader(m_pVertexShader);
			for (size_t i = 0; i < m_pVertexShader->m_vBufferIndexes.size(); i++)
			{
				auto pair = m_pVertexShader->m_vBufferIndexes[i];
				ConstantBuffer* constBuffer = m_pEffect->m_ConstantBuffers[pair.first];
				m_pDevice->SetConstantBuffer(ShaderStage::Vertex, i, constBuffer);
			}
		}

		if (m_pPixelShader)
		{
			m_pDevice->SetPixelShader(m_pPixelShader);
			for (size_t i = 0; i < m_pPixelShader->m_vBufferIndexes.size(); i++)
			{
				auto pair = m_pPixelShader->m_vBufferIndexes[i];
				ConstantBuffer* constBuffer = m_pEffect->m_ConstantBuffers[pair.first];
				m_pDevice->SetConstantBuffer(ShaderStage::Pixel, i, constBuffer);
			}
		}
#endif
	}
}