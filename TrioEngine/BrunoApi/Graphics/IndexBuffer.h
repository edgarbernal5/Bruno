#pragma once

#include "TrioApiRequisites.h"

#include "BindFlags.h"
#include "CpuAccessFlags.h"
#include "IndexElementSize.h"
#include "MapMode.h"
#include "ResourceUsage.h"
#include "ResourceOptionFlags.h"
#include "SetDataOptions.h"

namespace TrioEngine
{
	class GraphicsDevice;

	class BRUNO_API_EXPORT IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(GraphicsDevice* device, IndexElementSize elementSize, int indexCount, ResourceUsage usage);
		IndexBuffer(GraphicsDevice* device, IndexElementSize elementSize, int indexCount);
		virtual	~IndexBuffer();

		template <typename T>
		void GetData(int offsetInBytes, T* data, int length, int startIndex, int elementCount);

		template <typename T>
		void GetData(T* data, int length);

		inline int GetIndexCount() { return m_indexCount; }
		inline IndexElementSize GetElementSize() { return m_elementSize; }

		template <typename T>
		void SetData(const T* data, int length);

		template <typename T>
		void SetData(const T* data, int length, int startIndex, int elementCount);

		template <typename T>
		void SetData(int offsetInBytes, const T* data, int length, int startIndex, int elementCount, SetDataOptions options);

		friend class GraphicsDevice;

	private:
		GraphicsDevice*				m_device;
		int							m_indexCount;
		IndexElementSize			m_elementSize;
		ResourceUsage				m_usage;

#ifdef BRUNO_DIRECTX
		ID3D11Buffer*				m_buffer;
#elif BRUNO_OPENGL
		GLuint						m_buffer;
#endif

#ifdef BRUNO_DIRECTX
		void CreateBuffer(D3D11_SUBRESOURCE_DATA* subdata);
#endif
	};

	template <typename T>
	void IndexBuffer::GetData(int offsetInBytes, T* data, int length, int startIndex, int elementCount)
	{
		if (length < (startIndex + elementCount))
			return;
		//if (m_eUsage == ResourceUsage::Dynamic)
		//TO-DO:
		//	return;

#ifdef BRUNO_DIRECTX
		uint32_t elementSizeInBytes = m_elementSize == IndexElementSize::SixteenBits ? 2 : 4;

		D3D11_BUFFER_DESC stagingDesc;
		m_buffer->GetDesc(&stagingDesc);

		stagingDesc.BindFlags = (uint32_t)BindFlags::None;
		stagingDesc.CPUAccessFlags = (uint32_t)(CpuAccessFlags::Read | CpuAccessFlags::Write);
		stagingDesc.Usage = (D3D11_USAGE)ResourceUsage::Staging;
		stagingDesc.MiscFlags = (uint32_t)ResourceOptionFlags::None;

		ID3D11Buffer* stagingBuffer = nullptr;
		DX::ThrowIfFailed(
			m_device->GetD3DDevice()->CreateBuffer(&stagingDesc, nullptr, &stagingBuffer)
		);

		m_device->GetD3DDeviceContext()->CopyResource(stagingBuffer, m_buffer);

		D3D11_MAPPED_SUBRESOURCE box;
		m_device->GetD3DDeviceContext()->Map(stagingBuffer, 0, (D3D11_MAP)MapMode::Read, 0, &box);

		T* pData = reinterpret_cast<T*>(box.pData) + (offsetInBytes / sizeof(T));
		memcpy(data + startIndex, pData, elementSizeInBytes * elementCount);

		m_device->GetD3DDeviceContext()->Unmap(stagingBuffer, 0);

		RELEASE_COM(stagingBuffer);
#elif BRUNO_OPENGL
		uint32_t elementSizeInBytes = m_eElementSize == IndexElementSize::SixteenBits ? 2 : 4;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);

		uint8_t* dataInBytes = (uint8_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		dataInBytes += offsetInBytes;

		T* dataT = (T*)dataInBytes;

		memcpy(data + startIndex, dataT, elementSizeInBytes * elementCount);

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif
	}

	template <typename T>
	void IndexBuffer::GetData(T* data, int length)
	{
		GetData<T>(0, data, length, 0, length);
	}

	template <typename T>
	void IndexBuffer::SetData(int offsetInBytes, const T* data, int length, int startIndex, int elementCount, SetDataOptions options)
	{
		int elementSizeInBytes = m_elementSize == IndexElementSize::SixteenBits ? 2 : 4;

#ifdef BRUNO_DIRECTX
		switch (m_usage)
		{
		case ResourceUsage::Immutable:
			if (m_buffer == nullptr)
			{
				D3D11_SUBRESOURCE_DATA iinitData = { 0 };
				iinitData.pSysMem = data;
				CreateBuffer(&iinitData);
			}
			else
			{
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
				CreateBuffer(nullptr);
			}
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
#elif BRUNO_OPENGL
		bool first = true;
		uint32_t sizeInBytes = elementCount * elementSizeInBytes;
		uint32_t bufferSize = m_iIndexCount * elementSizeInBytes;
		if (m_buffer == 0)
		{
			first = false;
			glGenBuffers(1, &m_buffer);
			GraphicsExtensions::checkGLError("Indexbuffer glGenBuffers");
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, (m_eUsage == ResourceUsage::Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
			GraphicsExtensions::checkGLError("Indexbuffer glBufferData m_buffer == 0");
		}

		if (first) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
			GraphicsExtensions::checkGLError("Indexbuffer first=true");
		}
		if (options == SetDataOptions::Discard)
		{
			// By assigning NULL data to the buffer this gives a hint
			// to the device to discard the previous content.
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, nullptr, (m_eUsage == ResourceUsage::Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
			GraphicsExtensions::checkGLError("Indexbuffer Discard");
		}
		uint8_t* dataPTR = ((uint8_t*)(data)) + startIndex * elementSizeInBytes;
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetInBytes, sizeInBytes, dataPTR);
		std::stringstream ss;
		ss << "elementSizeInBytes " << elementSizeInBytes << endl;
		ss << "m_buffer " << m_buffer << endl;
		ss << "offsetInBytes " << offsetInBytes << endl;
		ss << "bufferSize " << bufferSize << endl;
		ss << "sizeInBytes " << sizeInBytes << endl;
		ss << "direccion " << (int)data << endl;
		ss << "m_eUsage " << (int)m_eUsage << endl;
		ss << "Indexbuffer main glBufferSubData" << endl;
		for (size_t i = 0; i < 16; i++)
		{
			ss << " " << (int)dataPTR[i];
		}
		ss << endl;
		GraphicsExtensions::checkGLError((char*)ss.str().c_str());
#endif
	}

	template <typename T>
	void IndexBuffer::SetData(const T *data, int length)
	{
		SetData<T>(0, data, length, 0, length, SetDataOptions::None);
	}

	template <typename T>
	void IndexBuffer::SetData(const T *data, int length, int startIndex, int elementCount)
	{
		SetData<T>(0, data, length, startIndex, elementCount, SetDataOptions::None);
	}

	///////////////////////////////////////////////////////////////////////
	class BRUNO_API_EXPORT DynamicIndexBuffer : public IndexBuffer
	{
	public:
		DynamicIndexBuffer(GraphicsDevice* device, IndexElementSize elementSize, int indexCount);
		~DynamicIndexBuffer();

		friend class GraphicsDevice;
	private:
		int m_userOffset;
	};
}

