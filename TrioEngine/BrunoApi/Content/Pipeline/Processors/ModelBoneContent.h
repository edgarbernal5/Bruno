#pragma once

#include "TrioApiRequisites.h"

#include "Math/MathVector.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class BRUNO_API_EXPORT ModelBoneContent 
	{
	public:
		ModelBoneContent();
		ModelBoneContent(std::string name, int index, Matrix transform, ModelBoneContent* parent);
		~ModelBoneContent();

		inline int GetIndex()
		{
			return m_index;
		}
		inline const std::string& GetName()
		{
			return m_name;
		}
		inline const Matrix& GetTransform()
		{
			return m_transform;
		}
		inline ModelBoneContent* GetParent()
		{
			return m_parent;
		}
		inline const std::vector<ModelBoneContent*>& GetChildren()
		{
			return m_chidren;
		}

		void SetChildren(std::vector<ModelBoneContent*>& children)
		{
			m_chidren.clear();
			m_chidren = std::vector<ModelBoneContent*>(children.begin(), children.end());
		}
	private:
		std::vector<ModelBoneContent*> m_chidren;
		int m_index;
		std::string m_name;
		ModelBoneContent* m_parent;
		Matrix m_transform;
	};
}