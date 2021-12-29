#pragma once

#include "TrioApiRequisites.h"

#include "Math/MathVector.h"

#include <string>
#include <vector>

namespace BrunoEngine
{
	class MeshContent;
	class ModelBoneContent;
	class SceneSubMeshContent;

	class BRUNO_API_EXPORT SceneMeshContent
	{
	public:
		SceneMeshContent();
		SceneMeshContent(std::string name, MeshContent* sourceMesh, ModelBoneContent* parentBone/*, BoundingSphere boundingSphere, BoundingBox boundingBox*/, std::vector<SceneSubMeshContent*>& meshParts);
		~SceneMeshContent();

		inline std::string GetName()
		{
			return m_name;
		}

		inline const std::vector<SceneSubMeshContent*>& GetSubMeshes()
		{
			return m_meshParts;
		}

		inline ModelBoneContent* GetParentBone()
		{
			return m_parentBone;
		}

		//inline const BoundingSphere& GetBoundingSphere()
		//{
		//	return m_boundingSphere;
		//}
		//inline const BoundingBox& GetBoundingBox()
		//{
		//	return m_boundingBox;
		//}
	private:
		std::string m_name;
		std::vector<SceneSubMeshContent*> m_meshParts;
		ModelBoneContent* m_parentBone;
		MeshContent* m_sourceMesh;

		/*BoundingSphere m_boundingSphere;
		BoundingBox m_boundingBox;*/
	};
}