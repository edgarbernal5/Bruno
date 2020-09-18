#pragma once

#include "TrioApiRequisites.h"

#include "FormatHelper.h"
#include "VertexElement.h"
#include "..\Utils\Hash.h"

#include <vector>
#include <map>

namespace TrioEngine
{
	//class VertexElement;
	class GraphicsDevice;
	class Shader;

#if TRIO_OPENGL
	struct VertexDeclarationAttributeInfo
	{
		struct Element
		{
			int Offset;
			int AttributeLocation;
			int NumberOfElements;
			GLenum VertexAttribPointerType;
			bool Normalized;

			Element(){}
			Element(int _offset, int _attributeLocation, int _numberOfElements, GLenum _vertexAttribPointerType, bool _normalized) : 
				Offset(_offset), AttributeLocation(_attributeLocation), NumberOfElements(_numberOfElements), VertexAttribPointerType(_vertexAttribPointerType), Normalized(_normalized){}
		};
		std::vector<Element> Elements;
		std::vector<bool> EnabledAttributes;
		std::vector<int> AttributesIndexes;

		VertexDeclarationAttributeInfo(){}
		VertexDeclarationAttributeInfo(int maxVertexAttributes)
		{
			EnabledAttributes.resize(maxVertexAttributes);
			AttributesIndexes.resize(maxVertexAttributes);
			for (size_t i = 0; i < maxVertexAttributes; i++)
			{
				EnabledAttributes[i] = false;
				AttributesIndexes[i] = -1;
			}
		}


	};
#endif

	class TRIO_API_EXPORT VertexDeclaration
	{
	public:
		VertexDeclaration();
		VertexDeclaration(int vertexStride, std::vector<VertexElement>& elements);
		VertexDeclaration(std::vector<VertexElement>& elements);

		~VertexDeclaration();

		void CalculateHashKey();

		inline int GetVertexStride()	{ return m_vertexStride; }
		inline int GetHashKey()			{ return m_iHashKey; }

		friend class GraphicsDevice;
		friend class InputLayoutCache;
		friend class VertexDeclarationHelper;
	private:
#ifdef TRIO_DIRECTX
		VertexDeclaration(D3D11_INPUT_ELEMENT_DESC const* dxElements, VertexElement const* pElements, int size);
#endif
		VertexDeclaration(VertexElement const* pElements, int size);

		int m_vertexStride;
		std::vector<VertexElement> m_elements;
		int m_iHashKey;

#ifdef TRIO_DIRECTX
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_d3d11Elements;
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetD3D11InputElements();
#elif TRIO_OPENGL
		std::map<int, VertexDeclarationAttributeInfo> m_shaderAttributeInfo;

		int OpenGLNumberOfElements(VertexElementFormat elementFormat);
		GLenum OpenGLVertexAttribPointerType(VertexElementFormat elementFormat);

		bool OpenGLVertexAttribNormalized(VertexElement element);
		void Apply(Shader* shader, int offset);

		GraphicsDevice* m_device;
#endif

	};
}
