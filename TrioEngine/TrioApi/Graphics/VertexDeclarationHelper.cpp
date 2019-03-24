#include "stdafx.h"
#include "VertexDeclarationHelper.h"

#include "VertexDeclaration.h"
#include "VertexElement.h"

namespace Vago
{
	std::map<std::string, VertexDeclaration*> VertexDeclarationHelper::g_declarationByClass;

#ifdef TRIO_DIRECTX
	VertexDeclaration* VertexDeclarationHelper::GetVertexDeclaration(const int inputElementCount, const D3D11_INPUT_ELEMENT_DESC *d3dInputElements, const VertexElement *inputElements)
	{
		/*std::vector<VertexElement> elements;
		int offset = 0;
		for (int i = 0; i < inputElementCount; i++)
		{
			VertexElement item;
			D3D11_INPUT_ELEMENT_DESC temp(inputElements[i]);
			temp.AlignedByteOffset = offset;

			item.GetVertexElementFromD3D11(temp);

			elements.push_back(item);

			offset += GetTypeSize(temp.Format);
		}*/

		return new VertexDeclaration(d3dInputElements, inputElements, inputElementCount);
}
#elif TRIO_OPENGL
	VertexDeclaration* VertexDeclarationHelper::GetVertexDeclaration(const int inputElementCount, const VertexElement *inputElements)
	{
		return new VertexDeclaration(std::vector<VertexElement>(inputElements, inputElements + inputElementCount));
	}
#endif

	VertexDeclaration* VertexDeclarationHelper::GetVertexDeclarationByClassName(std::string className)
	{
		auto it = g_declarationByClass.find(className);
		if (it == g_declarationByClass.end())
		{
			return nullptr;
		}

		return it->second; 
	}

	void VertexDeclarationHelper::AddVertexDeclaration(std::string className, VertexDeclaration* decl)
	{
		g_declarationByClass[className] = decl;
	}
}