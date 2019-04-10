#include "stdafx.h"
#include "TextureCollection.h"

#include "GraphicsDevice.h"
#include "Texture.h"

namespace Vago
{
	TextureCollection::TextureCollection(ShaderStage stage) :
		m_eStage(stage)
	{
		CreateArray();
#ifdef TRIO_OPENGL
		for (size_t i = 0; i < 16; i++)
		{
			m_glTargets[i] = 0;
		}
#endif
	}

	TextureCollection::~TextureCollection()
	{
	}

	void TextureCollection::CreateArray()
	{
		m_iN = 16;
		for (int i = 0; i < m_iN; i++)
		{
			m_pTextures[i] = nullptr;
		}
		m_iDirty = 0; //INT_MAX;
	}
	
	void TextureCollection::SetTexture(int index, Texture* tex)
	{
		//if (m_pTextures[index] == tex && m_pTextures[index] != nullptr)
		//	return;

		m_pTextures[index] = tex;
		m_iDirty |= 1 << index;
	}
	
	void TextureCollection::BindAllTextures(GraphicsDevice* device)
	{
		if (m_iDirty == 0)
			return;

		//TO-DO: tomar en cuenta las texturas de vertex shader
		for (int i = 0; i < m_iN; i++)
        {
			int mask = 1 << i;
			if ((m_iDirty & mask) == 0)
				continue;
			
			Texture* texture = m_pTextures[i];
#ifdef TRIO_DIRECTX
			if (texture == nullptr)
			{
				switch (m_eStage)
				{
				case ShaderStage::Vertex:
					device->GetD3DDeviceContext()->VSSetShaderResources(i, 1, nullptr);
					break;
				case ShaderStage::Pixel:
					device->GetD3DDeviceContext()->PSSetShaderResources(i, 1, nullptr);
					break;
				case ShaderStage::Geometry:
					break;
				}
			}
			else
			{
				ID3D11ShaderResourceView* view = texture->GetShaderResourceView();
				switch (m_eStage)
				{
				case ShaderStage::Vertex:
					device->GetD3DDeviceContext()->VSSetShaderResources(i, 1, &view);
					break;
				case ShaderStage::Pixel:
					device->GetD3DDeviceContext()->PSSetShaderResources(i, 1, &view);
					break;
				case ShaderStage::Geometry:
					break;
				}
			}
#elif TRIO_OPENGL

			// Clear the previous binding if the 
			// target is different from the new one.
			if (m_glTargets[i] != 0 && (tex == nullptr || m_glTargets[i] != tex->m_glTarget))
			{
				glBindTexture(m_glTargets[i], 0);
				m_glTargets[i] = 0;
			}

			if (tex != nullptr)
			{
				m_glTargets[i] = tex->m_glTarget;
				glBindTexture(tex->m_glTarget, tex->m_glTexture);
			}
#endif
			
			m_iDirty &= ~mask;
			if (m_iDirty == 0)
				break;
		}
		m_iDirty = 0;
	}
}