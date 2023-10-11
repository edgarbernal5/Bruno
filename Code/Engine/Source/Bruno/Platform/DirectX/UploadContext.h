#pragma once

#include "Context.h"
#include <vector>

namespace Bruno
{
	class GPUBuffer;
	struct BufferUpload;
	struct TextureUpload;
	class Texture;

	class UploadContext : public Context
	{
	public:
		UploadContext(GraphicsDevice& device, std::unique_ptr<GPUBuffer> bufferUploadHeap, std::unique_ptr<GPUBuffer> textureUploadHeap);
		~UploadContext();

		void AddBufferUpload(std::unique_ptr<BufferUpload> bufferUpload);
		void AddTextureUpload(std::unique_ptr<TextureUpload> textureUpload);

		void ProcessUploads();
		void ResolveProcessedUploads();

	private:
		std::unique_ptr<GPUBuffer> mBufferUploadHeap;
		std::unique_ptr<GPUBuffer> mTextureUploadHeap;

		std::vector<std::unique_ptr<BufferUpload>> mBufferUploads;
		std::vector<std::unique_ptr<TextureUpload>> mTextureUploads;

		std::vector<GPUBuffer*> mBufferUploadsInProgress;
		std::vector<Texture*> mTextureUploadsInProgress;
	};
}

