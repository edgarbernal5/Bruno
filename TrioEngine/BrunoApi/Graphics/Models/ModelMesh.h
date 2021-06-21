#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include "Math/MathCollision.h"

namespace BrunoEngine
{
	class ModelBone;
	class ModelMeshPart;

	class BRUNO_API_EXPORT ModelMesh
	{
	public:
		ModelMesh();
		ModelMesh(std::string name, ModelBone* parentBone, Matrix& transform, BoundingSphere boundingSphere, BoundingBox boundingBox, std::vector<ModelMeshPart*>& meshParts);
		~ModelMesh();

		inline const std::vector<ModelMeshPart*>& GetModelMeshParts()
		{
			return m_modelMeshParts;
		}

		inline ModelBone* GetParentBone()
		{
			return m_parentBone;
		}

		const char* GetName() {
			return m_name.c_str();
		}

		inline Matrix& GetTransform() {
			return m_transform;
		}

		inline const BoundingSphere& GetBoundingSphere()
		{
			return m_boundingSphere;
		}
		inline const BoundingBox& GetBoundingBox()
		{
			return m_boundingBox;
		}

	private:
		std::string m_name;
		std::vector<ModelMeshPart*> m_modelMeshParts;
		ModelBone* m_parentBone;
		Matrix m_transform;

		BoundingSphere m_boundingSphere;
		BoundingBox m_boundingBox;
	};
}