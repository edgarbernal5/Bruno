#pragma once

#include "TrioApiRequisites.h"
#include "Texture.h"

#include "../Math/Rectangle.h"
#include "ResourceUsage.h"
#include "CpuAccessFlags.h"
#include "MapMode.h"
#include "BindFlags.h"
#include "ResourceOptionFlags.h"

#include <string>

namespace TrioEngine
{
	class GraphicsDevice;

	class TRIO_API_EXPORT Texture2D : public Texture
	{
	public:
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height);
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height, SurfaceFormat format);
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format);

		~Texture2D();

		inline int GetWidth() { return m_width; }
		inline int GetHeight() { return m_height; }

		template <class T>
		void GetData(T* data, int length);

		template <class T>
		void GetData(int level, Rectangle* rect, T* data, int length, int startIndex, int elementCount);

		ResourceUsage GetUsage() { return m_usage; }

		template <class T>
		void SetData(T* data, int length);

		template <class T>
		void SetData(T* data, int length, int startIndex, int elementCount);

		template <class T>
		void SetData(int level, Rectangle* rect, T* data, int length, int startIndex, int elementCount);

		void SetUsage(ResourceUsage usage) { m_usage = usage; }

		//friend class SpriteBatch;
	protected:
		Texture2D(GraphicsDevice* graphicsDevice, int width, int height, uint32_t mipmap, SurfaceFormat format, SurfaceType surfaceType, uint32_t multiSamples, uint32_t msQuality, bool shared);

		int m_width;
		int m_height;

		uint32_t m_arraySize;
		uint32_t m_multiSamples;
		uint32_t m_msQuality;

		ResourceUsage m_usage;
		SurfaceType m_surfaceType;
		bool m_shared;

		void CommonConstructor(int width, int height, uint32_t mipmap, SurfaceType type, uint32_t multiSamples, uint32_t msQuality, bool shared, uint32_t uArraySize);

#ifdef TRIO_DIRECTX
		ID3D11Texture2D* m_stagingTex;
		void CreateTexture(D3D11_SUBRESOURCE_DATA* subdata);

		D3D11_TEXTURE2D_DESC m_tex2DDesc;
#endif
	private:

	};

	template <class T>
	void Texture2D::GetData(T* data, int length)
	{
		GetData<T>(0, nullptr, data, length, 0, length);
	}

	template <class T>
	void Texture2D::GetData(int level, Rectangle* rect, T* data, int length, int startIndex, int elementCount)
	{
		if (data == nullptr)
			return;

		int x, y, w, h;
		if (rect != nullptr)
		{
			x = rect->x;
			y = rect->y;
			w = rect->width;
			h = rect->height;
		}
		else
		{
			x = 0;
			y = 0;
			w = std::max<int>(m_width >> level, 1);
			h = std::max<int>(m_height >> level, 1);

			//TO-DO: modificar h y w con formatos Dxt1, etc.
			// Ref: http://www.mentby.com/Group/mac-opengl/issue-with-dxt-mipmapped-textures.html 
			if (m_format == SurfaceFormat::Dxt1 ||
				m_format == SurfaceFormat::Dxt1a ||
				m_format == SurfaceFormat::Dxt3 ||
				m_format == SurfaceFormat::Dxt5)
			{
				w = (w + 3) & ~3;
				h = (h + 3) & ~3;
			}
		}

#ifdef TRIO_DIRECTX
		if (m_stagingTex == nullptr)
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Usage = (D3D11_USAGE)ResourceUsage::Staging;
			desc.Width = m_width;
			desc.Height = m_height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = ToFormat(m_format);
			desc.BindFlags = (UINT)BindFlags::None;
			desc.CPUAccessFlags = (UINT)CpuAccessFlags::Read;
			desc.MiscFlags = (UINT)ResourceOptionFlags::None;
			desc.SampleDesc.Count = m_multiSamples;
			desc.SampleDesc.Quality = m_msQuality;

			//TO-DO: hacer un pool de texturas staging.
			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateTexture2D(&desc, nullptr, &m_stagingTex)
			);
		}

		D3D11_BOX region;
		region.front = 0;
		region.back = 1;

		region.top = y;
		region.bottom = y + h;
		region.left = x;
		region.right = x + w;

		m_device->GetD3DDeviceContext()->CopySubresourceRegion(m_stagingTex, 0, 0, 0, 0, m_texture, level, &region);

		D3D11_MAPPED_SUBRESOURCE mapsource;
		m_device->GetD3DDeviceContext()->Map(m_stagingTex, 0, (D3D11_MAP)MapMode::Read, 0, &mapsource);

		T *pData = reinterpret_cast<T*>(mapsource.pData);
		memcpy(data + startIndex, pData + startIndex, sizeof(T) * elementCount);

		m_device->GetD3DDeviceContext()->Unmap(m_stagingTex, 0);
#endif
	}

	template <class T>
	void Texture2D::SetData(int level, Rectangle * rect, T * data, int length, int startIndex, int elementCount)
	{
		int x, y, w, h;
		if (rect != nullptr)
		{
			x = rect->x;
			y = rect->y;
			w = rect->width;
			h = rect->height;
		}
		else
		{
			x = 0;
			y = 0;
			w = std::max<int>(m_width >> level, 1);
			h = std::max<int>(m_height >> level, 1);

			//TO-DO: modificar h y w con formatos Dxt1, etc.
			// Ref: http://www.mentby.com/Group/mac-opengl/issue-with-dxt-mipmapped-textures.html 
			if (m_format == SurfaceFormat::Dxt1 ||
				m_format == SurfaceFormat::Dxt1a ||
				m_format == SurfaceFormat::Dxt3 ||
				m_format == SurfaceFormat::Dxt5)
			{
				w = (w + 3) & ~3;
				h = (h + 3) & ~3;
			}
		}
#ifdef TRIO_DIRECTX
		switch (m_usage)
		{
		case TrioEngine::ResourceUsage::Dynamic:
		case TrioEngine::ResourceUsage::Default:
			if (m_texture == nullptr)
			{
				CreateTexture(nullptr);
			}

			D3D11_BOX region;

			region.front = 0;
			region.back = 1;

			region.top = y;
			region.bottom = y + h;
			region.left = x;
			region.right = x + w;

			//TO-DO: Revisar esto bien.
			m_device->GetD3DDeviceContext()->UpdateSubresource(m_texture, level, &region, data + startIndex, GetPitch(w), 0);

			break;
		case TrioEngine::ResourceUsage::Immutable:

			if (m_texture == nullptr)
			{
				D3D11_SUBRESOURCE_DATA iinitData = { 0 };
				iinitData.pSysMem = data;

				CreateTexture(&iinitData);
			}
			else
			{
				D3D11_BOX region;

				region.front = 0;
				region.back = 1;

				region.top = y;
				region.bottom = y + h;
				region.left = x;
				region.right = x + w;

				//TO-DO: Revisar esto bien.
				m_device->GetD3DDeviceContext()->UpdateSubresource(m_texture, level, &region, data + startIndex, GetPitch(w), 0);
			}
			break;
		case TrioEngine::ResourceUsage::Staging:
			break;
		default:
			break;
		}

		
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