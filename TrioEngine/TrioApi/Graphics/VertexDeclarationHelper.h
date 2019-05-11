#pragma once

#include "TrioApiDecl.h"

#include <string>
#include <vector>
#include <map>

namespace TrioEngine
{
	class VertexDeclaration;

	class TRIOAPI_DECL VertexDeclarationHelper 
	{
	public:
#ifdef TRIO_DIRECTX
		static VertexDeclaration* GetVertexDeclaration(const int InputElementCount, const D3D11_INPUT_ELEMENT_DESC *d3dInputElements, const VertexElement *inputElements);
#elif TRIO_OPENGL
		static VertexDeclaration* GetVertexDeclaration(const int InputElementCount, const VertexElement *InputElements);
#endif

		static VertexDeclaration* GetVertexDeclarationByClassName(std::string className);
		static void AddVertexDeclaration(std::string className, VertexDeclaration* decl);
	private:
		static std::map<std::string, VertexDeclaration*> g_declarationByClass;
	};
}