#pragma once

#include "TrioApiRequisites.h"

#include "ShaderLanguageTarget.h"
#include <string>
#include <vector>
#include <map>

namespace TrioEngine
{
	class EffectParameter;

	class TRIO_API_EXPORT EffectParameterCollection
	{
	public:
		EffectParameterCollection();
		EffectParameterCollection(std::vector<EffectParameter*> parameters);
		~EffectParameterCollection();

		typedef EffectParameter* BaseType;

		const BaseType & operator[](std::string nName) const;

		BaseType & operator[](std::string nName);

		const BaseType & operator[](int nIndex) const;

		BaseType & operator[](int nIndex);

		void clear();
		void push_back(EffectParameter* parameter);
		inline size_t size();
	private:
		std::map<std::string, BaseType> m_data;
		std::vector<BaseType> m_array;
	};
}