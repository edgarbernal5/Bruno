#pragma once

#include "TrioApiDecl.h"

#include <string>

struct aiMesh;

namespace TrioEngine
{
	class Model;
	
	class TRIOAPI_DECL Mesh
	{
	public:
		
		friend class Model;
	private:
		Mesh(Model& model, aiMesh& mesh);

		std::string m_csName;
		uint32_t m_uiFaceCount;
	};
}