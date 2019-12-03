#pragma once

#include "TrioApiRequisites.h"

#include <vector>
#include <string>
#include <map>

namespace TrioEngine
{
#ifdef TRIO_DIRECTX
	class GraphicsDevice;
	class VertexDeclaration;

	class TRIO_API_EXPORT InputLayoutCache
	{
	public:
		InputLayoutCache(GraphicsDevice* device, std::vector<uint8_t>& byteCode);
		~InputLayoutCache();

		ID3D11InputLayout* Get(VertexDeclaration* vertexDecl);
	private:
		std::map<VertexDeclaration*, ID3D11InputLayout*> m_cache;
		
		GraphicsDevice* m_device;

		std::vector<uint8_t>& m_byteCode;
	};
#endif
}

