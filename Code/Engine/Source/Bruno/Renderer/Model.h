#pragma once

#include "Bruno/Core/RTTI.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"

#include <string>

namespace Bruno
{
	class Mesh;

	class Model : public RTTI
	{
		BR_RTTI_DECLARATION(Model, RTTI);

	public:
		//Model(std::vector<Mesh>&& meshes);

	private:
		std::vector<Mesh> m_meshes;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		std::shared_ptr<VertexBuffer> VertexB;
		std::shared_ptr<IndexBuffer> IndexB;
	};
}
