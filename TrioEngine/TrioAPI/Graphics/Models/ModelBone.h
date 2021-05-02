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

		friend class Model;

		friend class ModelReader;

	private:
		Matrix m_transform;
		ModelBone* m_parent;

		std::vector<ModelBone*> m_children;
		uint32_t m_index;
	};
}