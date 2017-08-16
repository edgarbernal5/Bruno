#pragma once

#include "TrioAPI.h"

#include <string>
#include <map>
#include <vector>

namespace Cuado
{

	class TRIOAPI_DLL EffectPass;

	class TRIOAPI_DLL EffectPassCollection
	{
	public:
		EffectPassCollection();
		EffectPassCollection(std::vector<EffectPass*> parameters);
		~EffectPassCollection();

		typedef EffectPass* BaseType;

		void clear();

		const BaseType & operator[](std::string nName) const;

		BaseType & operator[](std::string nName);

		const BaseType & operator[](int nIndex) const;

		BaseType & operator[](int nIndex);

		inline size_t size();
	private:
		std::map<std::string, BaseType> m_Data;
		std::vector<BaseType> m_array;
	};
}