#pragma once

#include "TrioApiDecl.h"

#include "../Math/MathVector.h"
#include <string>

struct aiMesh;

namespace TrioEngine
{
	class Model;
	class ModelMaterial;
	
	class TRIOAPI_DECL Mesh
	{
	public:
		
		friend class Model;
	private:
		Mesh(Model& model, aiMesh& mesh);

		std::string m_name;
		Model* m_model;
		ModelMaterial* m_material;
		uint32_t m_faceCount;
		std::vector<uint32_t> m_indices;

		std::vector<Vector3> m_vertices;
		std::vector<Vector3> m_normals;
		std::vector<Vector3> m_tangents;
		std::vector<Vector3> m_binormals;
		std::vector<std::vector<Vector3>*> m_textureCoordinates;
		std::vector<std::vector<Vector4>*> m_vertexColors;
	};
}