#include "stdafx.h"
#include "VertexElement.h"

namespace Vago
{
	VertexElement::VertexElement()
	{
	}

	VertexElement::VertexElement(int offset, VertexElementFormat elementFormat, VertexElementUsage elementUsage, int usageIndex) :
		m_iOffset(offset),
		m_eFormat(elementFormat),
		m_eUsage(elementUsage),
		m_iUsageIndex(usageIndex)
	{
	}

	VertexElement::~VertexElement()
	{
	}

#ifdef TRIO_DIRECTX
	void VertexElement::GetVertexElementFromD3D11(D3D11_INPUT_ELEMENT_DESC desc)
	{
		m_eUsage = VertexElementUsage::Position;

		if (strcmp("SV_Position", desc.SemanticName) == 0 || strcmp("POSITION", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Position;
		else if (strcmp("COLOR", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Color;
		else if (strcmp("TEXCOORD", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::TextureCoordinate;
		else if (strcmp("NORMAL", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Normal;
		else if (strcmp("BINORMAL", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Binormal;
		else if (strcmp("TANGENT", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Tangent;
		else if (strcmp("BLENDINDICES", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::BlendIndices;
		else if (strcmp("BLENDWEIGHT", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::BlendWeight;
		else if (strcmp("DEPTH", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::Depth;
		else if (strcmp("PSIZE", desc.SemanticName) == 0)
			m_eUsage = VertexElementUsage::PointSize;

		m_iUsageIndex = desc.SemanticIndex;

		switch (desc.Format)
		{
		case DXGI_FORMAT_R32_FLOAT:
			m_eFormat = VertexElementFormat::Single;
			break;
		case DXGI_FORMAT_R32G32_FLOAT:
			m_eFormat = VertexElementFormat::Vector2;
			break;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			m_eFormat = VertexElementFormat::Vector3;
			break;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			m_eFormat = VertexElementFormat::Vector4;
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			m_eFormat = VertexElementFormat::Color;
			break;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			m_eFormat = VertexElementFormat::Byte4;
			break;
		case DXGI_FORMAT_R16G16_SINT:
			m_eFormat = VertexElementFormat::Short2;
			break;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			m_eFormat = VertexElementFormat::Short4;
			break;
		case DXGI_FORMAT_R16G16_SNORM:
			m_eFormat = VertexElementFormat::NormalizedShort2;
			break;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			m_eFormat = VertexElementFormat::NormalizedShort4;
			break;
		case DXGI_FORMAT_R16G16_FLOAT:
			m_eFormat = VertexElementFormat::HalfVector2;
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			m_eFormat = VertexElementFormat::HalfVector4;
			break;
		default:
			m_eFormat = VertexElementFormat::Color;
			break;
		}
		m_iOffset = desc.AlignedByteOffset;
	}

	D3D11_INPUT_ELEMENT_DESC VertexElement::GetD3D11InputElement()
	{
		D3D11_INPUT_ELEMENT_DESC element;

		switch (m_eUsage)
		{
		case VertexElementUsage::Position:
			element.SemanticName = "POSITION";
			break;
		case VertexElementUsage::Color:
			element.SemanticName = "COLOR";
			break;
		case VertexElementUsage::TextureCoordinate:
			element.SemanticName = "TEXCOORD";
			break;
		case VertexElementUsage::Normal:
			element.SemanticName = "NORMAL";
			break;
		case VertexElementUsage::Binormal:
			element.SemanticName = "BINORMAL";
			break;
		case VertexElementUsage::Tangent:
			element.SemanticName = "TANGENT";
			break;
		case VertexElementUsage::BlendIndices:
			element.SemanticName = "BLENDINDICES";
			break;
		case VertexElementUsage::BlendWeight:
			element.SemanticName = "BLENDWEIGHT";
			break;
		case VertexElementUsage::Depth:
			element.SemanticName = "DEPTH";
			break;
		case VertexElementUsage::PointSize:
			element.SemanticName = "PSIZE";
			break;
		default:
			break;
		}

		element.SemanticIndex = m_iUsageIndex;

		switch (m_eFormat)
		{
		case VertexElementFormat::Single:
			element.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case VertexElementFormat::Vector2:
			element.Format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case VertexElementFormat::Vector3:
			element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case VertexElementFormat::Vector4:
			element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VertexElementFormat::Color:
			element.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case VertexElementFormat::Byte4:
			element.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			break;
		case VertexElementFormat::Short2:
			element.Format = DXGI_FORMAT_R16G16_SINT;
			break;
		case VertexElementFormat::Short4:
			element.Format = DXGI_FORMAT_R16G16B16A16_SINT;
			break;
		case VertexElementFormat::NormalizedShort2:
			element.Format = DXGI_FORMAT_R16G16_SNORM;
			break;
		case VertexElementFormat::NormalizedShort4:
			element.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
			break;
		case VertexElementFormat::HalfVector2:
			element.Format = DXGI_FORMAT_R16G16_FLOAT;
			break;
		case VertexElementFormat::HalfVector4:
			element.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		default:
			break;
		}

		element.InputSlot = 0;
		element.AlignedByteOffset = m_iOffset;

		// Note that instancing is only supported in 
		// feature level 9.3 and above.
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;

		return element;
	}
#endif
}