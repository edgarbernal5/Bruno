#include "brpch.h"
#include "UploadContext.h"

#include "GPUBuffer.h"
#include "Texture.h"
#include "Bruno/Core/Memory.h"

namespace Bruno
{
	UploadContext::UploadContext(GraphicsDevice& device, std::unique_ptr<GPUBuffer> bufferUploadHeap, std::unique_ptr<GPUBuffer> textureUploadHeap) :
		Context(device, D3D12_COMMAND_LIST_TYPE_COPY),
        mBufferUploadHeap(std::move(bufferUploadHeap)),
        mTextureUploadHeap(std::move(textureUploadHeap))
	{
	}
	
	UploadContext::~UploadContext()
	{
	}

	void UploadContext::AddBufferUpload(std::unique_ptr<BufferUpload> bufferUpload)
	{
		BR_ASSERT(bufferUpload->mBufferDataSize <= mBufferUploadHeap->mDesc.Width);

		mBufferUploads.push_back(std::move(bufferUpload));
	}

	void UploadContext::AddTextureUpload(std::unique_ptr<TextureUpload> textureUpload)
	{
		BR_ASSERT(textureUpload->mTextureDataSize <= mTextureUploadHeap->mDesc.Width);

		mTextureUploads.push_back(std::move(textureUpload));
	}
	
	void UploadContext::ProcessUploads()
	{
		const uint32_t numBufferUploads = static_cast<uint32_t>(mBufferUploads.size());
		const uint32_t numTextureUploads = static_cast<uint32_t>(mTextureUploads.size());
		uint32_t numBuffersProcessed = 0;
		uint32_t numTexturesProcessed = 0;
		size_t bufferUploadHeapOffset = 0;
		size_t textureUploadHeapOffset = 0;

        for (numBuffersProcessed; numBuffersProcessed < numBufferUploads; ++numBuffersProcessed)
        {
            BufferUpload& currentUpload = *mBufferUploads[numBuffersProcessed];

            if ((bufferUploadHeapOffset + currentUpload.mBufferDataSize) > mBufferUploadHeap->mDesc.Width)
            {
                break;
            }

            memcpy(mBufferUploadHeap->mMappedResource + bufferUploadHeapOffset, currentUpload.mBufferData.get(), currentUpload.mBufferDataSize);
            CopyBufferRegion(*currentUpload.mBuffer, 0, *mBufferUploadHeap, bufferUploadHeapOffset, currentUpload.mBufferDataSize);

            bufferUploadHeapOffset += currentUpload.mBufferDataSize;
            mBufferUploadsInProgress.push_back(currentUpload.mBuffer);
        }

        for (numTexturesProcessed; numTexturesProcessed < numTextureUploads; ++numTexturesProcessed)
        {
            TextureUpload& currentUpload = *mTextureUploads[numTexturesProcessed];

            if ((textureUploadHeapOffset + currentUpload.mTextureDataSize) > mTextureUploadHeap->mDesc.Width)
            {
                break;
            }

            memcpy(mTextureUploadHeap->mMappedResource + textureUploadHeapOffset, currentUpload.mTextureData.get(), currentUpload.mTextureDataSize);
            CopyTextureRegion(*currentUpload.mTexture, *mTextureUploadHeap, textureUploadHeapOffset, currentUpload.mSubResourceLayouts, currentUpload.mNumSubResources);

            textureUploadHeapOffset += currentUpload.mTextureDataSize;
            textureUploadHeapOffset = AlignU64(textureUploadHeapOffset, 512);

            mTextureUploadsInProgress.push_back(currentUpload.mTexture);
        }

        if (numBuffersProcessed > 0)
        {
            mBufferUploads.erase(mBufferUploads.begin(), mBufferUploads.begin() + numBuffersProcessed);
        }

        if (numTexturesProcessed > 0)
        {
            mTextureUploads.erase(mTextureUploads.begin(), mTextureUploads.begin() + numTexturesProcessed);
        }
	}
    
    void UploadContext::ResolveProcessedUploads()
    {
        for (auto& bufferUploadInProgress : mBufferUploadsInProgress)
        {
            bufferUploadInProgress->mIsReady = true;
        }

        for (auto& textureUploadInProgress : mTextureUploadsInProgress)
        {
            textureUploadInProgress->mIsReady = true;
        }

        mBufferUploadsInProgress.clear();
        mTextureUploadsInProgress.clear();
    }
}