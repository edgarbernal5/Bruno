#pragma once

#include "Context.h"
#include <vector>

namespace Bruno
{
	class GpuBuffer;
	struct BufferUpload;
	struct TextureUpload;
	class Texture;

	class UploadContext : public Context
	{
	public:
		UploadContext(GraphicsDevice& device, std::unique_ptr<GpuBuffer> bufferUploadHeap, std::unique_ptr<GpuBuffer> textureUploadHeap);
		~UploadContext();

		void AddBufferUpload(std::unique_ptr<BufferUpload> bufferUpload);
		void AddTextureUpload(std::unique_ptr<TextureUpload> textureUpload);

		void ProcessUploads();
		void ResolveProcessedUploads();

	private:
		std::unique_ptr<GpuBuffer> m_bufferUploadHeap;
		std::unique_ptr<GpuBuffer> m_textureUploadHeap;

		std::vector<std::unique_ptr<BufferUpload>> m_bufferUploads;
		std::vector<std::unique_ptr<TextureUpload>> m_textureUploads;

		std::vector<GpuBuffer*> m_bufferUploadsInProgress;
		std::vector<Texture*> m_textureUploadsInProgress;
	};
}

