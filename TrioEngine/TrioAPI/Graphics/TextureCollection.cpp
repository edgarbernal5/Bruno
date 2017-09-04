#include "stdafx.h"
#include "TextureCollection.h"

#include "GraphicsDevice.h"
namespace Cuado
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
		m_pTextures = new Texture*[m_iN];
		for (int i = 0; i < m_iN; i++)
		{
			m_pTextures[i] = nullptr;
		}
		m_iDirty = INT_MAX;
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
				//device->m_d3dContext->PSSetShaderResources(i, 1, nullptr);
			}
			else
			{
				ID3D11ShaderResourceView* view = texture->GetShaderResourceView();
				switch (m_eStage)
				{
				case Cuado::ShaderStage::Vertex:
					device->GetD3DDeviceContext()->VSSetShaderResources(i, 1, &view);
					break;
				case Cuado::ShaderStage::Pixel:
					device->GetD3DDeviceContext()->PSSetShaderResources(i, 1, &view);
					break;
				case Cuado::ShaderStage::Geometry:
					break;
				}
			}
#elif TRIO_OPENGL
			glActiveTexture(GL_TEXTURE0 + i);
			CHECK_GL_ERROR(glActiveTexture);

			// Clear the previous binding if the 
			// target is different from the new one.
			if (m_glTargets[i] != 0 && (texture == nullptr || m_glTargets[i] != texture->m_glTarget))
			{
				glBindTexture(m_glTargets[i], 0);
				CHECK_GL_ERROR(glBindTexture);
				m_glTargets[i] = 0;
			}

			if (texture != nullptr)
			{
				m_glTargets[i] = texture->m_glTarget;
				glBindTexture(texture->m_glTarget, texture->m_glTexture);
				CHECK_GL_ERROR(glBindTexture);
			}
#endif
			
			m_iDirty &= ~mask;
			if (m_iDirty == 0)
				break;
		}
		m_iDirty = 0;
	}
}