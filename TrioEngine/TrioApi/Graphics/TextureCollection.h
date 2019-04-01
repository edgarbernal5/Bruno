#pragma once

#include "TrioAPI.h"

#include "ShaderStage.h"

namespace Vago
{
	class Texture;
	class GraphicsDevice;

	class TRIOAPI_DECL TextureCollection
	{
	public:
		typedef Texture* BaseType;

		TextureCollection(ShaderStage stage);
		~TextureCollection();
		
		void BindAllTextures(GraphicsDevice* m_Device);
		void SetTexture(int index, Texture* tex);

		const BaseType & operator[](int iIndex) const
		{
			return m_pTextures[iIndex];
		}

		BaseType & operator[](int iIndex)
		{
			m_iDirty |= 1 << iIndex;
			return m_pTextures[iIndex];
		}
	private:
#ifdef TRIO_OPENGL
		GLenum		m_glTargets[16];
#endif

		Texture*	m_pTextures[16];
		//array<Texture *, 16> m_pTextures;

		int			m_iDirty;
		int			m_iN;
		ShaderStage m_eStage;

		void CreateArray();

		//BaseType & GetAt(int iIndex)
		//{
		//	assert( iIndex >= 0 && iIndex < 16 ); 
		//	return m_pTextures[iIndex];
		//}
	};
}
