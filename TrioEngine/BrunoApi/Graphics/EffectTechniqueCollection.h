#pragma once

#include "TrioApiRequisites.h"

#include "ShaderLanguageTarget.h"
#include <string>
#include <vector>
#include <map>

namespace TrioEngine
{
	class EffectTechnique;

	class BRUNO_API_EXPORT EffectTechniqueCollection
	{
	public:
		EffectTechniqueCollection();
		EffectTechniqueCollection(std::vector<EffectTechnique*> parameters);
		~EffectTechniqueCollection();

		typedef EffectTechnique* BaseType;

		const BaseType & operator[](std::string nName) const;

		BaseType & operator[](std::string nName);

		const BaseType & operator[](int nIndex) const;

		BaseType & operator[](int nIndex);

		void clear();
		void push_back(EffectTechnique* technique);
		inline size_t size();

	private:
		std::map<std::string, BaseType> m_data;
		std::vector<BaseType> m_array;
	};
}