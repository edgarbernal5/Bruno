#pragma once

#include "TrioApiDecl.h"

#include <string>

namespace TrioEngine
{
	class Mesh;
	
	class TRIOAPI_DECL Model
	{
	public:
		struct ModelLoadParams
		{
			bool flipUVs;
			ModelLoadParams() :flipUVs(false) {}
		};

		Model(const std::string& filename, ModelLoadParams loadParams = ModelLoadParams());
		~Model();

		const std::vector<Mesh*>& GetMeshes() const;
	private:


		std::vector<Mesh*> m_vMeshes;
	};
}