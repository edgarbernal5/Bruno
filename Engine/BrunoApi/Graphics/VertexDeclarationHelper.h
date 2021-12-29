#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>
#include <map>

namespace BrunoEngine
{
	class VertexDeclaration;

	class BRUNO_API_EXPORT VertexDeclarationHelper 
	{
	public:
#ifdef BRUNO_DIRECTX
		static VertexDeclaration* GetVertexDeclaration(const int InputElementCount, const D3D11_INPUT_ELEMENT_DESC *d3dInputElements, const VertexElement *inputElements);
#elif BRUNO_OPENGL
		static VertexDeclaration* GetVertexDeclaration(const int InputElementCount, const VertexElement *InputElements);
#endif

		static VertexDeclaration* GetVertexDeclarationByClassName(std::string className);
		static void AddVertexDeclaration(std::string className, VertexDeclaration* decl);
	private:
		static std::map<std::string, VertexDeclaration*> g_declarationByClass;
	};
}