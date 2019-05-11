#pragma once

#include "TrioApiDecl.h"

namespace TrioEngine
{
	enum class ResourceUsage : int
	{
		//Lectura y escritura en la GPU (ej: Depth buffer)
#ifdef TRIO_DIRECTX
		Default = D3D11_USAGE_DEFAULT,
#else
		Default,
#endif

		//El contenido del buffer no cambiará luego de crearse.
#ifdef TRIO_DIRECTX
		Immutable = D3D11_USAGE_IMMUTABLE,
#else
		Immutable,
#endif

		//A resource that is accessible by both the GPU (read only) and the CPU (write only). A dynamic resource is a good choice for a resource that will be updated by the CPU at least once per frame
#ifdef TRIO_DIRECTX
		Dynamic = D3D11_USAGE_DYNAMIC,
#else
		Dynamic,
#endif

#ifdef TRIO_DIRECTX
		//A resource that supports data transfer (copy) from the GPU to the CPU.
		Staging = D3D11_USAGE_STAGING
#else
		Staging,
#endif
	};
}
