#pragma once

#include "TrioAPI.h"

#include <string>
#include <map>
#include <vector>

namespace Cuado
{

	class TRIOAPI_DLL EffectParameter;

	class TRIOAPI_DLL EffectParameterCollection
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
		std::map<std::string, BaseType> m_Data;
		std::vector<BaseType> m_array;
	};
}