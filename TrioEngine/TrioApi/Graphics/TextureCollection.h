#pragma once

#include "TrioAPI.h"

#include "ShaderStage.h"

namespace TrioEngine
{
	class Texture;
	class GraphicsDevice;

	class TRIO_API_EXPORT TextureCollection
	{
	public:
		typedef Texture* BaseType;

		TextureCollection(ShaderStage stage);
		~TextureCollection();
		
		void BindAllTextures(GraphicsDevice* m_Device);
		void SetTexture(int index, Texture* tex);

		const BaseType & operator[](int iIndex) const
		{
			return m_textures[iIndex];
		}

		BaseType & operator[](int iIndex)
		{
			m_dirty |= 1 << iIndex;
			return m_textures[iIndex];
		}
	private:
#ifdef TRIO_OPENGL
		GLenum		m_glTargets[16];
#endif

		Texture*	m_textures[16];
		//array<Texture *, 16> m_pTextures;

		int			m_dirty;
		int			m_n;
		ShaderStage m_stage;

		void CreateArray();

		//BaseType & GetAt(int iIndex)
		//{
		//	assert( iIndex >= 0 && iIndex < 16 ); 
		//	return m_pTextures[iIndex];
		//}
	};
}
