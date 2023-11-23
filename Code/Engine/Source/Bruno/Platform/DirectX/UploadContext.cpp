#include "brpch.h"
#include "UploadContext.h"

#include "GpuBuffer.h"
#include "Texture.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
	UploadContext::UploadContext(GraphicsDevice& device, std::unique_ptr<GpuBuffer> bufferUploadHeap, std::unique_ptr<GpuBuffer> textureUploadHeap) :
		Context(device, D3D12_COMMAND_LIST_TYPE_COPY),
        m_bufferUploadHeap(std::move(bufferUploadHeap)),
        m_textureUploadHeap(std::move(textureUploadHeap))
	{
	}
	
	UploadContext::~UploadContext()
	{
	}

	void UploadContext::AddBufferUpload(std::unique_ptr<BufferUpload> bufferUpload)
	{
		BR_ASSERT(bufferUpload->BufferDataSize <= m_bufferUploadHeap->m_desc.Width);

		m_bufferUploads.push_back(std::move(bufferUpload));
	}

	void UploadContext::AddTextureUpload(std::unique_ptr<TextureUpload> textureUpload)
	{
		BR_ASSERT(textureUpload->TextureDataSize <= m_textureUploadHeap->m_desc.Width);

		m_textureUploads.push_back(std::move(textureUpload));
	}
	
	void UploadContext::ProcessUploads()
	{
		const uint32_t numBufferUploads = static_cast<uint32_t>(m_bufferUploads.size());
		const uint32_t numTextureUploads = static_cast<uint32_t>(m_textureUploads.size());
		uint32_t numBuffersProcessed = 0;
		uint32_t numTexturesProcessed = 0;
		size_t bufferUploadHeapOffset = 0;
		size_t textureUploadHeapOffset = 0;

        for (numBuffersProcessed; numBuffersProcessed < numBufferUploads; ++numBuffersProcessed)
        {
            BufferUpload& currentUpload = *m_bufferUploads[numBuffersProcessed];

            if ((bufferUploadHeapOffset + currentUpload.BufferDataSize) > m_bufferUploadHeap->m_desc.Width)
            {
                break;
            }

            memcpy(m_bufferUploadHeap->m_mappedResource + bufferUploadHeapOffset, currentUpload.BufferData.get(), currentUpload.BufferDataSize);
            CopyBufferRegion(*currentUpload.Buffer, 0, *m_bufferUploadHeap, bufferUploadHeapOffset, currentUpload.BufferDataSize);

            bufferUploadHeapOffset += currentUpload.BufferDataSize;
            m_bufferUploadsInProgress.push_back(currentUpload.Buffer);
        }

        for (numTexturesProcessed; numTexturesProcessed < numTextureUploads; ++numTexturesProcessed)
        {
            TextureUpload& currentUpload = *m_textureUploads[numTexturesProcessed];

            if ((textureUploadHeapOffset + currentUpload.TextureDataSize) > m_textureUploadHeap->m_desc.Width)
            {
                break;
            }

            memcpy(m_textureUploadHeap->m_mappedResource + textureUploadHeapOffset, currentUpload.TextureData.get(), currentUpload.TextureDataSize);
            CopyTextureRegion(*currentUpload.Texture, *m_textureUploadHeap, textureUploadHeapOffset, currentUpload.SubResourceLayouts, currentUpload.SubResourcesCount);

            textureUploadHeapOffset += currentUpload.TextureDataSize;
            textureUploadHeapOffset = AlignU64(textureUploadHeapOffset, 512);

            m_textureUploadsInProgress.push_back(currentUpload.Texture);
        }

        if (numBuffersProcessed > 0)
        {
            m_bufferUploads.erase(m_bufferUploads.begin(), m_bufferUploads.begin() + numBuffersProcessed);
        }

        if (numTexturesProcessed > 0)
        {
            m_textureUploads.erase(m_textureUploads.begin(), m_textureUploads.begin() + numTexturesProcessed);
        }
	}
    
    void UploadContext::ResolveProcessedUploads()
    {
        for (auto& bufferUploadInProgress : m_bufferUploadsInProgress)
        {
            bufferUploadInProgress->m_isReady = true;
        }

        for (auto& textureUploadInProgress : m_textureUploadsInProgress)
        {
            textureUploadInProgress->m_isReady = true;
        }

        m_bufferUploadsInProgress.clear();
        m_textureUploadsInProgress.clear();
    }
}