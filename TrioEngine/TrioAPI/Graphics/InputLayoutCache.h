#pragma once

#if TRIO_DIRECTX
#include "TrioAPI.h"

#include <vector>
#include <string>
#include <map>

namespace Cuado
{
	class TRIOAPI_DLL GraphicsDevice;

	class TRIOAPI_DLL InputLayoutCache
	{
	public:
		InputLayoutCache(GraphicsDevice* device, std::vector<uint8_t>& byteCode);
		~InputLayoutCache();

		ID3D11InputLayout* Get(VertexDeclaration* vertexDecl);
	private:
		std::map<VertexDeclaration*, ID3D11InputLayout*> m_cache;
		
		GraphicsDevice* m_pDevice;

		std::vector<uint8_t>& m_vByteCode;
	};
}

#endif
