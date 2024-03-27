#include "brpch.h"
#include "Material.h"

#include "Bruno/Platform/DirectX/Texture.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Material);

	Material::Material()
	{
		m_handle = {};
	}
}