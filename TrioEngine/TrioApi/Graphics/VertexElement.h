#pragma once

#include "DepthFormat.h"
#include "IndexElementSize.h"
#include "PresentInterval.h"
#include "SurfaceFormat.h"
#include "VertexElementFormat.h"
#include "VertexElementUsage.h"

#include <sstream>

namespace TrioEngine
{
	class VertexDeclaration;

	//TO-DO: agregar las nuevas características de DX11
	class VertexElement
	{
	public:
		VertexElement();
		VertexElement(int offset, VertexElementFormat elementFormat, VertexElementUsage elementUsage, int usageIndex);
		~VertexElement();

		friend class VertexDeclaration;
		friend class VertexDeclarationHelper;

		inline int GetOffset() { return m_offset; }
		inline VertexElementFormat GetVertexElementFormat() { return m_format; }
		inline VertexElementUsage GetVertexElementUsage() { return m_usage; }
		inline int GetUsageIndex() { return m_iUsageIndex; }

		inline void SetOffset(int offset) { m_offset = offset; }
		inline void SetVertexElementFormat(VertexElementFormat format) { m_format = format; }
		inline void SetVertexElementUsage(VertexElementUsage usage) { m_usage = usage; }
		inline void SetUsageIndex(int usageIndex) { m_iUsageIndex = usageIndex; }

		bool operator ==(const VertexElement& other) const
		{
			return m_offset == other.m_offset &&
				m_format == other.m_format &&
				m_usage == other.m_usage &&
				m_iUsageIndex == other.m_iUsageIndex;
		}

		bool operator !=(const VertexElement& other) const
		{
			return !(*this == other);
		}

	private:
		int m_offset;
		VertexElementFormat m_format;
		VertexElementUsage m_usage;
		int m_iUsageIndex;

#ifdef TRIO_DIRECTX
		D3D11_INPUT_ELEMENT_DESC GetD3D11InputElement();
		void GetVertexElementFromD3D11(D3D11_INPUT_ELEMENT_DESC desc);
#endif

		std::string GetSignature()
		{
			std::stringstream ss;
			ss << "(Offset " << m_offset << ", Format " << (int)m_format << ", Usage " << (int)m_usage << ", UsageIndex " << m_iUsageIndex << ")";

			return ss.str();
		}
	};
}