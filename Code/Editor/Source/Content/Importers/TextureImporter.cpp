#include "brepch.h"
#include "TextureImporter.h"

#include <Bruno/Platform/DirectX/Texture.h>

#include "Bruno/Platform/DirectX/CommandQueueManager.h"
#include "Bruno/Platform/DirectX/Device.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/Queue.h"
#include "Bruno/Platform/DirectX/Texture2D.h"
#include "Bruno/Platform/DirectX/UploadContext_Gem.h"

namespace Bruno
{
	bool TextureImporter::TryImport(const AssetMetadata& metadata, AssetImporterContext& context, std::shared_ptr<Asset>& asset)
	{
		auto assetFilename = context.GetAbsolutePath(metadata.Filename);
		
		auto device = Graphics::GetDXDevice();
		
		auto& uploadContext = device->GetUploadContext();
		auto& commandManager = Graphics::GetCommandQueueManager();
		
		uploadContext.Reset();
		
		asset = std::make_shared<DX::Texture2D>(*device, uploadContext, device->GetSRVDescriptorAllocator(), assetFilename);
		asset->SetHandle(metadata.Handle);

		uint64_t uploadFence = commandManager->ExecuteAndReturnFence(uploadContext);

		commandManager->WaitForGpuFence(uploadFence);

		uploadContext.ClearGarbage();

		return true;
	}
}
