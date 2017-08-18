#pragma once

#include "TrioAPI.h"

#include <string>
#include <vector>


namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	class TRIOAPI_DLL IVertexType
	{
	public:
		virtual ~IVertexType() {}

		//virtual VertexDeclaration* GetVertexDeclaration() = 0;
	};
}