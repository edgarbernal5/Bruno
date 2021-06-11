#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace TrioEngine
{
	class TRIO_API_EXPORT ModelBone
	{
	public:
		ModelBone();
		ModelBone(std::string name, Matrix transform, int index);
		~ModelBone();

		inline uint32_t GetIndex()
		{
			return m_index;
		}

		inline const char* GetName()
		{
			return m_name.c_str();
		}


		inline Matrix GetAbsoluteTransform()
		{
			if (m_parent){
				return m_transform * m_parent->GetAbsoluteTransform();
			}
			return m_transform;
		}

		inline Matrix& GetTransform()
		{
			return m_transform;
		}

		inline ModelBone* GetParentBone()
		{
			return m_parent;
		}
		friend class Model;

		friend class ModelReader;
		friend class SceneReader;

	private:
		Matrix m_transform;
		ModelBone* m_parent;

		std::vector<ModelBone*> m_children;
		uint32_t m_index;
		std::string m_name;
	};
}