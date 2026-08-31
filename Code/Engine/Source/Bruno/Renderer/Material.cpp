#include "brpch.h"
#include "Material.h"

#include "Bruno/Content/AssetManager.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/Texture2D.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Material);

	Material::Material() : m_textureDescriptorAllocation()
	{
		m_handle = {};
	}
}
