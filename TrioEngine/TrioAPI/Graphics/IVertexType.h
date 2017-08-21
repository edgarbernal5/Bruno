#pragma once

#include "TrioAPI.h"

#include <string>
#include <vector>


namespace Cuado
{
	class TRIOAPI_DLL VertexDeclaration;

	struct TRIOAPI_DLL IVertexType
	{
	public:
		IVertexType(){ }
		//virtual ~IVertexType() {}

		//virtual VertexDeclaration* GetVertexDeclaration() = 0;
	};
}