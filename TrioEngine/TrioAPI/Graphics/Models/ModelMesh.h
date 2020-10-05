#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class ModelBone;
	class ModelMeshPart;

	class TRIO_API_EXPORT ModelMesh
	{
	public:
		ModelMesh();
		ModelMesh(std::string name, ModelBone* parentBone/*, BoundingSphere boundingSphere, BoundingBox boundingBox*/, std::vector<ModelMeshPart*>& meshParts);
		~ModelMesh();

		inline const std::vector<ModelMeshPart*>& GetModelMeshParts()
		{
			return m_ModelMeshParts;
		}
		inline ModelBone* GetParentBone()
		{
			return m_parentBone;
		}

		/*inline const BoundingSphere& GetBoundingSphere()
		{
			return m_boundingSphere;
		}
		inline const BoundingBox& GetBoundingBox()
		{
			return m_boundingBox;
		}*/

	private:
		std::string m_name;
		std::vector<ModelMeshPart*> m_ModelMeshParts;
		ModelBone* m_parentBone;

		/*BoundingSphere m_boundingSphere;
		BoundingBox m_boundingBox;*/
	};
}