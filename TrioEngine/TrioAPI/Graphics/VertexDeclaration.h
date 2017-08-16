#pragma once

#include "TrioAPI.h"


#include "FormatHelper.h"
#include "VertexElement.h"
#include "Utilities.h"

#include <vector>
#include <map>

namespace Cuado
{
	class TRIOAPI_DLL VertexElement;
	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL Shader;

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

	class TRIOAPI_DLL VertexDeclaration
	{
	public:
		VertexDeclaration();
		VertexDeclaration(int vertexStride, std::vector<VertexElement>& elements);
		VertexDeclaration(std::vector<VertexElement>& elements);
		~VertexDeclaration();

		void CalculateHashKey();

		inline int GetVertexStride()	{ return m_iVertexStride; }
		inline int GetHashKey()			{ return m_iHashKey; }

		friend class GraphicsDevice;
	private:
		int m_iVertexStride;
		std::vector<VertexElement> m_vElements;

		int m_iHashKey;

#ifdef TRIO_DIRECTX
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements();
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
