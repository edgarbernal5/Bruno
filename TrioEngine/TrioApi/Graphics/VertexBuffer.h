#pragma once

#include "TrioApiDecl.h"

#include "BindFlags.h"
#include "ResourceUsage.h"
#include "MapMode.h"
#include "VertexBufferBinding.h"
#include "VertexDeclaration.h"
#include "SetDataOptions.h"
#include "VertexBufferBinding.h"

namespace TrioEngine
{
	class VertexDeclaration;

	class TRIOAPI_DECL VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount, ResourceUsage usage);
		
		VertexBuffer(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount);
		virtual ~VertexBuffer();

		template <typename T>
		void GetData(int offsetInBytes, T* data, int length, int startIndex, int elementCount);

		template <typename T>
		void GetData(T* data, int length);

		inline int GetVertexCount() { return m_vertexCount; }
		inline VertexDeclaration* GetVertexDeclaration() { return m_vertexDeclaration; }

		template <class T>
		void SetData(const T* data, int length);

		template <class T>
		void SetData(int offsetInBytes, const T* data, int length, int startIndex, int elementCount, int vertexStride, SetDataOptions options);

		friend class GraphicsDevice;
	private:
#ifdef TRIO_DIRECTX
		ID3D11Buffer* m_buffer;
		//ID3D11InputLayout* m_InputLayout;

		void CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata);
#elif OPENGL
		GLuint m_pBuffer;
#endif
		GraphicsDevice * m_device;

		VertexDeclaration *m_vertexDeclaration;
		ResourceUsage m_usage;
		VertexBufferBinding m_binding;

		int m_vertexCount;

		friend class GraphicsDevice;
	protected:
	};

	template <typename T>
	void VertexBuffer::GetData(int offsetInBytes, T* data, int length, int startIndex, int elementCount)
	{
		if (length < (startIndex + elementCount))
			return;

		//if (m_eUsage == ResourceUsage::Dynamic)
		//TO-DO: implementar.
		//return;

#ifdef TRIO_DIRECTX
		uint32_t elementSizeInBytes = m_vertexDeclaration->GetVertexStride();
		D3D11_BUFFER_DESC stagingDesc;
		m_buffer->GetDesc(&stagingDesc);

		stagingDesc.BindFlags = (uint32_t)BindFlags::None;
		stagingDesc.CPUAccessFlags = (uint32_t)(CpuAccessFlags::Read | CpuAccessFlags::Write);
		stagingDesc.Usage = (D3D11_USAGE)ResourceUsage::Staging;
		stagingDesc.MiscFlags = (uint32_t)ResourceOptionFlags::None;

		ID3D11Buffer* stagingBuffer = nullptr;
		DX::ThrowIfFailed(m_device->GetD3DDevice()->CreateBuffer(&stagingDesc, nullptr, &stagingBuffer));

		m_device->GetD3DDeviceContext()->CopyResource(stagingBuffer, m_buffer);

		D3D11_MAPPED_SUBRESOURCE box;
		m_device->GetD3DDeviceContext()->Map(stagingBuffer, 0, (D3D11_MAP)MapMode::Read, 0, &box);

		T *pData = reinterpret_cast<T*>(box.pData) + (offsetInBytes / sizeof(T));
		memcpy(data + startIndex, pData, elementSizeInBytes * elementCount);

		m_device->GetD3DDeviceContext()->Unmap(stagingBuffer, 0);

		RELEASE_COM(stagingBuffer);
#elif TRIO_OPENGL
		uint32_t elementSizeInBytes = m_pVertexDeclaration->GetVertexStride();

		glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer);

		uint8_t* dataInBytes = (uint8_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		dataInBytes += offsetInBytes;

		T* dataT = (T*)dataInBytes;

		memcpy(data + startIndex, dataT, elementSizeInBytes * elementCount);

		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	template <typename T>
	void VertexBuffer::GetData(T* data, int length)
	{
		GetData(0, data, length, 0, length);
	}

	//TODO: agregar int elementSizeInBytes como parametro

	template <class T>
	void VertexBuffer::SetData(int offsetInBytes, const T* data, int length, int startIndex, int elementCount, int vertexStride, SetDataOptions options)
	{
		uint32_t elementSizeInBytes = vertexStride;
		if (elementSizeInBytes == 0) {
			elementSizeInBytes = sizeof(T);
		}
		//TO-DO: tomar en cuenta vertexStride menores a m_pVertexDeclaration->GetVertexStride().

		if (m_binding.Buffer == nullptr)
			m_binding = VertexBufferBinding(this, 0, m_vertexDeclaration->GetVertexStride());

#ifdef TRIO_DIRECTX
		switch (m_usage)
		{
		case ResourceUsage::Immutable:
			if (m_buffer == nullptr)
			{
				D3D11_SUBRESOURCE_DATA vinitData = { 0 };
				vinitData.pSysMem = data;

				CreateBuffer(&vinitData);
			}
			else
			{
				//usar un ResourceRegion.

				D3D11_BOX region;

				region.front = 0;
				region.back = 1;

				region.top = 0;
				region.bottom = 1;
				region.left = offsetInBytes;
				region.right = offsetInBytes + (elementCount * elementSizeInBytes);

				m_device->GetD3DDeviceContext()->UpdateSubresource(m_buffer, 0, &region, (data + startIndex), elementSizeInBytes, 0);
			}
			break;
		case ResourceUsage::Dynamic:
			if (m_buffer == nullptr)
			{
				D3D11_SUBRESOURCE_DATA vinitData = { 0 };
				vinitData.pSysMem = data;

				CreateBuffer(&vinitData);
			}
			else
			{
				MapMode mode = MapMode::WriteDiscard;
				if ((options & SetDataOptions::NoOverwrite) == SetDataOptions::NoOverwrite)
					mode = MapMode::WriteNoOverwrite;

				D3D11_MAPPED_SUBRESOURCE resource;

				m_device->GetD3DDeviceContext()->Map(m_buffer, 0, (D3D11_MAP)mode, 0, &resource);

				T* pDataResource = (T*)((uint8_t*)(resource.pData) + offsetInBytes);

				const T* pData = data + startIndex;

				memcpy(pDataResource, pData, elementSizeInBytes * elementCount);

				m_device->GetD3DDeviceContext()->Unmap(m_buffer, 0);
			}
			break;
		case ResourceUsage::Default:
			if (m_buffer == nullptr)
			{
				D3D11_SUBRESOURCE_DATA vinitData = { 0 };
				vinitData.pSysMem = data;

				CreateBuffer(&vinitData);
			}
			break;
		default:
			break;
		}
#elif TRIO_OPENGL
		bool first = true;
		uint32_t sizeInBytes = elementCount * elementSizeInBytes;
		//uint32_t sizeInBytes = elementCount * sizeof(T);
		uint32_t bufferSize = m_iVertexCount * m_pVertexDeclaration->GetVertexStride();

		if (m_pBuffer == 0)
		{
			first = false;
			glGenBuffers(1, &m_pBuffer);
			GraphicsExtensions::checkGLError("Vertexbuffer");
			glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer);
			GraphicsExtensions::checkGLError("Vertexbuffer");
			glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, (m_eUsage == ResourceUsage::Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
			GraphicsExtensions::checkGLError("Vertexbuffer");
		}

		if (first) {
			glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer);
			GraphicsExtensions::checkGLError("Vertexbuffer");
		}

		if (options == SetDataOptions::Discard)
		{
			// By assigning NULL data to the buffer this gives a hint
			// to the device to discard the previous content.
			glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, (m_eUsage == ResourceUsage::Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
			GraphicsExtensions::checkGLError("vs glBufferData");
		}

		//glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, sizeInBytes, (uint8_t*)(data)+startIndex * elementSizeInBytes);
		glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, sizeInBytes, data);
		GraphicsExtensions::checkGLError("vs glBufferSubData");
#endif
	}

	template <class T>
	void VertexBuffer::SetData(const T* data, int length)
	{
		SetData<T>(0, data, length, 0, length, 0, SetDataOptions::None);
	}

	class TRIOAPI_DECL DynamicVertexBuffer : public VertexBuffer
	{
	public:
		DynamicVertexBuffer(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount);
		~DynamicVertexBuffer();
		friend class GraphicsDevice;
		//friend class SpriteBatch;
		friend class SpriteBatcher;
	private:
		int m_userOffset;
	};
}

