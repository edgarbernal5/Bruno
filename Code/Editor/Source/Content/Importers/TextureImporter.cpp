#include "brepch.h"
#include "TextureImporter.h"

#include "Bruno/Platform/DirectX/CommandQueueManager.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/CommandQueue.h"
#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Platform/DirectX/UploadContext.h"

namespace Bruno
{
	bool TextureImporter::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& outputAsset)
	{
		auto assetFilename = context.GetAbsolutePath(metadata.Filename);
		
		auto device = Graphics::GetDevice();
		
		auto& uploadContext = device->GetUploadContext();
		auto& commandManager = Graphics::GetCommandQueueManager();
		
		uploadContext.Reset();
		
		outputAsset = std::make_shared<Texture2D>(*device, uploadContext, device->GetSRVDescriptorAllocator(), assetFilename);
		outputAsset->SetHandle(metadata.Handle);

		uint64_t uploadFence = commandManager->ExecuteAndReturnFence(uploadContext);

		commandManager->WaitForGpuFence(uploadFence);

		uploadContext.ClearGarbage();

		return true;
	}
}
