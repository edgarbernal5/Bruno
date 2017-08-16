#pragma once

#include "TrioAPI.h"

#include "Texture.h"
#include "StatesEnums.h"

namespace Cuado
{
	class TRIOAPI_DLL Texture;
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL TextureCollection
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
#ifdef OPENGL
		GLenum		m_glTargets[16];
#endif

		Texture**	m_pTextures;
		//array<Texture *, 16> m_pTextures;

		int			m_iDirty;
		int			m_iN;
		ShaderStage m_eStage;

		void CreateArray();

		BaseType & GetAt(int iIndex) const
		{
			assert( iIndex >= 0 && iIndex < 16 ); 
			return m_pTextures[iIndex];
		}
	};
}
