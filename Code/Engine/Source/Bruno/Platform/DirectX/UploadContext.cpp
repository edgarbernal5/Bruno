#include "brpch.h"
#include "UploadContext.h"

#include "GPUBuffer.h"
#include "Texture.h"

namespace Bruno
{
	UploadContext::UploadContext(GraphicsDevice& device, std::unique_ptr<GPUBuffer> bufferUploadHeap, std::unique_ptr<GPUBuffer> textureUploadHeap) :
		Context(device, D3D12_COMMAND_LIST_TYPE_COPY)
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

	}
}