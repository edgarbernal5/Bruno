#pragma once

#include "TrioApiDecl.h"

#include <vector>
#include <string>
#include <map>

namespace TrioEngine
{
#ifdef TRIO_DIRECTX
	class GraphicsDevice;
	class VertexDeclaration;

	class TRIOAPI_DECL InputLayoutCache
	{
	public:
		InputLayoutCache(GraphicsDevice* device, std::vector<uint8_t>& byteCode);
		~InputLayoutCache();

		ID3D11InputLayout* Get(VertexDeclaration* vertexDecl);
	private:
		std::map<VertexDeclaration*, ID3D11InputLayout*> m_mCache;
		
		GraphicsDevice* m_pDevice;

		std::vector<uint8_t>& m_vByteCode;
	};
#endif
}

