#pragma once

#include "TrioAPI.h"

#include <string>
#include <map>
#include <vector>

namespace Cuado
{

	class TRIOAPI_DLL EffectTechnique;

	class TRIOAPI_DLL EffectTechniqueCollection
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
		std::map<std::string, BaseType> m_Data;
		std::vector<BaseType> m_array;
	};
}