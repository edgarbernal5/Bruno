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

		inline int GetOffset() { return m_iOffset; }
		inline VertexElementFormat GetVertexElementFormat() { return m_eFormat; }
		inline VertexElementUsage GetVertexElementUsage() { return m_eUsage; }
		inline int GetUsageIndex() { return m_iUsageIndex; }

		inline void SetOffset(int offset) { m_iOffset = offset; }
		inline void SetVertexElementFormat(VertexElementFormat format) { m_eFormat = format; }
		inline void SetVertexElementUsage(VertexElementUsage usage) { m_eUsage = usage; }
		inline void SetUsageIndex(int usageIndex) { m_iUsageIndex = usageIndex; }

		bool operator ==(const VertexElement& other) const
		{
			return m_iOffset == other.m_iOffset &&
				m_eFormat == other.m_eFormat &&
				m_eUsage == other.m_eUsage &&
				m_iUsageIndex == other.m_iUsageIndex;
		}

		bool operator !=(const VertexElement& other) const
		{
			return !(*this == other);
		}

	private:
		int m_iOffset;
		VertexElementFormat m_eFormat;
		VertexElementUsage m_eUsage;
		int m_iUsageIndex;

#ifdef TRIO_DIRECTX
		D3D11_INPUT_ELEMENT_DESC GetD3D11InputElement();
		void GetVertexElementFromD3D11(D3D11_INPUT_ELEMENT_DESC desc);
#endif

		std::string GetSignature()
		{
			std::stringstream ss;
			ss << "(Offset " << m_iOffset << ", Format " << (int)m_eFormat << ", Usage " << (int)m_eUsage << ", UsageIndex " << m_iUsageIndex << ")";

			return ss.str();
		}
	};
}