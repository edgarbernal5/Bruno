#pragma once

#include "TrioAPI.h"
#include "Texture.h"

#include "Rectangles.h"
#include "ResourceEnums.h"
#include <string>

namespace Cuado
{
	class TRIOAPI_DLL Texture2D : public Texture
	{
	public:
		Texture2D();
		Texture2D(GraphicsDevice* graphicsDevice, std::string filename);
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height);
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format);

		~Texture2D();
		
		inline int GetWidth() { return m_iWidth; }
		inline int GetHeight() { return m_iHeight; }

		template <class T>
		void GetData(T* data, int length);

		template <class T>
		void GetData(int level, Rectangle * rect, T* data, int length, int startIndex, int elementCount);
		
		template <class T>
		void SetData(T* data, int length);
		
		template <class T>
		void SetData(T* data, int length, int startIndex, int elementCount);
		
		template <class T>
		void SetData(int level, Rectangle* rect, T* data, int length, int startIndex, int elementCount);


		//friend class SpriteBatch;
	protected:
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared);

		int m_iWidth;
		int m_iHeight;

		uint32_t m_uMultiSamples;
		uint32_t m_uMsQuality;

		ResourceUsage m_eUsage;
		SurfaceType m_eSurfaceType;
		bool m_bShared;

		void CommonConstructor(int width, int height, uint32_t mipmap, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared);

#ifdef TRIO_DIRECTX
		void CreateTexture(D3D11_SUBRESOURCE_DATA* subdata);
#endif
	private:
#ifdef TRIO_DIRECTX
		D3D11_TEXTURE2D_DESC m_Tex2DDesc;
#endif

	};
		
	template <class T>
	void Texture2D::GetData(T* data, int length)
	{
		GetData<T>(0, nullptr, data, length, 0, length);
	}

	template <class T>
	void Texture2D::GetData(int level, Rectangle * rect, T* data, int length, int startIndex, int elementCount)
	{
		if (data == nullptr)
			return;

		int x, y, w, h;
		if (rect != nullptr)
		{
			x = rect->X;
			y = rect->Y;
			w = rect->Width;
			h = rect->Height;
		}
		else
		{
			x = 0;
			y = 0;
			w = max((int)(m_Width >> level), 1);
			h = max((int)(m_Height >> level), 1);

			//TO-DO: modificar h y w con formatos Dxt1, etc.
			// Ref: http://www.mentby.com/Group/mac-opengl/issue-with-dxt-mipmapped-textures.html 
            if (m_Format == SurfaceFormat::Dxt1 ||
                m_Format == SurfaceFormat::Dxt1a ||
                m_Format == SurfaceFormat::Dxt3 ||
                m_Format == SurfaceFormat::Dxt5)
            {
                w = (w + 3) & ~3;
				h = (h + 3) & ~3;
            }
		}

#ifdef TRIO_DIRECTX
		D3D11_TEXTURE2D_DESC desc;
		desc.Usage = (D3D11_USAGE)ResourceUsage::Staging;
		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = ToFormat(m_Format);
		desc.BindFlags = (UINT)BindFlags::None;
		desc.CPUAccessFlags = (UINT)CpuAccessFlags::Read;
		desc.MiscFlags = (UINT)ResourceOptionFlags::None;

		desc.SampleDesc.Count = m_uMultiSamples;
		desc.SampleDesc.Quality = m_uMsQuality;

		//TO-DO: hacer un pool de texturas staging.
		ID3D11Texture2D *stagingTex = nullptr;
		DX::ThrowIfFailed(
			m_pDevice->GetD3DDevice()->CreateTexture2D(&desc, nullptr, &stagingTex)
		);
		
		D3D11_BOX region;
		region.front = 0;
		region.back = 1;

		region.top = y;
		region.bottom = y + h;
		region.left = x;
		region.right = x + w;

		m_pDevice->GetD3DDeviceContext()->CopySubresourceRegion(stagingTex, 0, 0, 0, 0, m_pTexture, level, &region);
		
		D3D11_MAPPED_SUBRESOURCE mapsource;
		m_pDevice->GetContext()->Map(stagingTex, 0, (D3D11_MAP)MapMode::Read, 0, &mapsource);
		
		T *pData = reinterpret_cast<T*>(mapsource.pData);
		memcpy(data + startIndex, pData + startIndex, sizeof(T) * elementCount);

		m_pDevice->GetD3DDeviceContext()->Unmap(stagingTex, 0);
		
		ReleaseCOM(stagingTex);
#endif
	}

	template <class T>
	void Texture2D::SetData(int level, Rectangle * rect, T * data, int length, int startIndex, int elementCount)
	{
		int x, y, w, h;
		if (rect != nullptr)
		{
			x = rect->X;
			y = rect->Y;
			w = rect->Width;
			h = rect->Height;
		}
		else
		{
			x = 0;
			y = 0;
			w = std::max((int)(m_Width >> level), 1);
			h = std::max((int)(m_Height >> level), 1);

			//TO-DO: modificar h y w con formatos Dxt1, etc.
			// Ref: http://www.mentby.com/Group/mac-opengl/issue-with-dxt-mipmapped-textures.html 
            if (m_Format == SurfaceFormat::Dxt1 ||
                m_Format == SurfaceFormat::Dxt1a ||
                m_Format == SurfaceFormat::Dxt3 ||
                m_Format == SurfaceFormat::Dxt5)
            {
                w = (w + 3) & ~3;
				h = (h + 3) & ~3;
            }
		}
#ifdef TRIO_DIRECTX
		if (m_pTexture == nullptr)
			CreateTexture(nullptr);
		
		D3D11_BOX region;
				
		region.front = 0;
		region.back = 1;

		region.top = y;
		region.bottom = y + h;
		region.left = x;
		region.right = x + w;

		//TO-DO: Revisar esto bien.
		m_pDevice->GetD3DDeviceContext()->UpdateSubresource(m_pTexture, level, &region, data + startIndex, GetPitch(w), 0);
#endif
	}

	template <class T>
	void Texture2D::SetData(T* data, int length)
	{
		SetData<T>(0, nullptr, data, length, 0, length);
	}
	
	template <class T>
	void Texture2D::SetData(T* data, int length, int startIndex, int elementCount)
	{
		SetData<T>(0, nullptr, data, length, startIndex, elementCount);
	}

	
}
