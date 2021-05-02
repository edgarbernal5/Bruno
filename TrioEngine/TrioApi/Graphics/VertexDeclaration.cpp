#include "stdafx.h"
#include "VertexDeclaration.h"

#include "Shader.h"
#include "GraphicsDevice.h"

namespace TrioEngine
{
	//Usar glVertexAttribPointer

	VertexDeclaration::VertexDeclaration() :
		m_vertexStride(0)
	{

	}

	VertexDeclaration::VertexDeclaration(int vertexStride, std::vector<VertexElement>& elements) :
		m_vertexStride(vertexStride),
		m_elements(elements)
	{
		m_iHashKey = 0;
		CalculateHashKey();
	}

	VertexDeclaration::VertexDeclaration(std::vector<VertexElement>& elements) :
		m_elements(elements)
	{
		m_iHashKey = 0;
		CalculateHashKey();
	}
#ifdef TRIO_DIRECTX
	VertexDeclaration::VertexDeclaration(D3D11_INPUT_ELEMENT_DESC const* dxElements, VertexElement const* pElements, int size) :
		m_elements(pElements, pElements + size),
		m_d3d11Elements(dxElements, dxElements + size)
	{
		m_iHashKey = 0;
		CalculateHashKey();
	}
#endif

	VertexDeclaration::VertexDeclaration(VertexElement const* pElements, int size) :
		m_elements(pElements, pElements + size)
	{

		m_iHashKey = 0;
		CalculateHashKey();
	}

	VertexDeclaration::~VertexDeclaration()
	{
	}

	void VertexDeclaration::CalculateHashKey()
	{
		std::string signature = "";
		m_vertexStride = 0;
		
		for (int i = 0; i < m_elements.size(); i++)
		{
			m_vertexStride += FormatHelper::GetByteSizeFrom(m_elements[i].m_format);

			signature += m_elements[i].GetSignature();
		}

		m_iHashKey = ComputeHash(signature.data(), signature.size());
	}

#ifdef TRIO_DIRECTX
	const std::vector<D3D11_INPUT_ELEMENT_DESC> & VertexDeclaration::GetD3D11InputElements()
	{
		if (m_d3d11Elements.size() == 0)
		{
			m_d3d11Elements.resize(m_elements.size());
			for (int i = 0; i < m_elements.size(); i++)
			{
				m_d3d11Elements[i] = m_elements[i].GetD3D11InputElement();
			}
		}
		return m_d3d11Elements;
	}

#elif TRIO_OPENGL
	void VertexDeclaration::Apply(Shader* shader, int offset)
	{
		VertexDeclarationAttributeInfo attrInfo;
		int shaderHash = shader->GetHashKey();
		map<int, VertexDeclarationAttributeInfo>::iterator itm = m_shaderAttributeInfo.find(shaderHash);
		if (itm == m_shaderAttributeInfo.end())
		{
			int maxVertexAttribs = 0;
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
			attrInfo = VertexDeclarationAttributeInfo(maxVertexAttribs);
			int indice = 0;

			for (auto &elem : m_vElements)
			{
				int attributeLocation = shader->GetAttribLocation(elem.GetVertexElementUsage(), elem.GetUsageIndex());

				if (attributeLocation >= 0)
				{
					attrInfo.Elements.push_back(VertexDeclarationAttributeInfo::Element
						(
						elem.GetOffset(),
						attributeLocation,
						OpenGLNumberOfElements(elem.GetVertexElementFormat()),
						OpenGLVertexAttribPointerType(elem.GetVertexElementFormat()),
						OpenGLVertexAttribNormalized(elem)
						)
					);
					attrInfo.EnabledAttributes[indice] = true;
					attrInfo.AttributesIndexes[indice] = attributeLocation;
					indice++;
				}
			}

			m_shaderAttributeInfo[shaderHash] = attrInfo;
		}
		else
		{
			attrInfo = itm->second;
		}
		//TO-DO: desahbilitar los vertexattribarray activos en este punto.
		for (size_t i = 0; i < attrInfo.Elements.size(); i++)
		{
			glDisableVertexAttribArray(i);
		}
		//m_device->SetVertexAttributeArray(attrInfo.EnabledAttributes, attrInfo.AttributesIndexes);

		for (auto &elem : attrInfo.Elements)
		{
			std::stringstream ss;
			
			glEnableVertexAttribArray(elem.AttributeLocation);
			
			glVertexAttribPointer
				(
					elem.AttributeLocation, elem.NumberOfElements, elem.VertexAttribPointerType, elem.Normalized ? GL_TRUE : GL_FALSE,
					m_iVertexStride, (void*)(elem.Offset + offset)
				);

			ss << "VertexDecl Apply " << elem.AttributeLocation << " " << elem.NumberOfElements << endl;
			
			GraphicsExtensions::checkGLError((char*)ss.str().c_str());
		}


	}

	int VertexDeclaration::OpenGLNumberOfElements(VertexElementFormat elementFormat)
	{
		switch (elementFormat)
		{
		case Cuado::VertexElementFormat::Single:
			return 1;
		case Cuado::VertexElementFormat::Vector2:
			return 2;
		case Cuado::VertexElementFormat::Vector3:
			return 3;
		case Cuado::VertexElementFormat::Vector4:
			return 4;
		case Cuado::VertexElementFormat::Color:
			return 4;
		case Cuado::VertexElementFormat::Byte4:
			return 4;
		case Cuado::VertexElementFormat::Short2:
			return 2;
		case Cuado::VertexElementFormat::Short4:
			return 2;
		case Cuado::VertexElementFormat::NormalizedShort2:
			return 2;
		case Cuado::VertexElementFormat::NormalizedShort4:
			return 4;
		case Cuado::VertexElementFormat::HalfVector2:
			return 2;
		case Cuado::VertexElementFormat::HalfVector4:
			return 4;
		default:
			return 0;
		}
	}


	bool VertexDeclaration::OpenGLVertexAttribNormalized(VertexElement element)
	{
		if (element.GetVertexElementUsage() == VertexElementUsage::Color)
			return true;

		switch (element.GetVertexElementFormat())
		{
		case VertexElementFormat::NormalizedShort2:
		case VertexElementFormat::NormalizedShort4:
			return true;
		}
		return false;
	}

	GLenum VertexDeclaration::OpenGLVertexAttribPointerType(VertexElementFormat elementFormat)
	{
		switch (elementFormat)
		{
		case Cuado::VertexElementFormat::Single:
			return GL_FLOAT;
		case Cuado::VertexElementFormat::Vector2:
			return GL_FLOAT;
		case Cuado::VertexElementFormat::Vector3:
			return GL_FLOAT;
		case Cuado::VertexElementFormat::Vector4:
			return GL_FLOAT;
		case Cuado::VertexElementFormat::Color:
			return GL_UNSIGNED_BYTE;
		case Cuado::VertexElementFormat::Byte4:
			return GL_UNSIGNED_BYTE;
		case Cuado::VertexElementFormat::Short2:
			return GL_SHORT;
		case Cuado::VertexElementFormat::Short4:
			return GL_SHORT;
		case Cuado::VertexElementFormat::NormalizedShort2:
			return GL_SHORT;
		case Cuado::VertexElementFormat::NormalizedShort4:
			return GL_SHORT;
		case Cuado::VertexElementFormat::HalfVector2:
			return GL_HALF_FLOAT;
		case Cuado::VertexElementFormat::HalfVector4:
			return GL_HALF_FLOAT;
		default:
			return GL_FLOAT;
		}
	}
#endif

	
}