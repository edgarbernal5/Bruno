#pragma once

#include "TrioApiDecl.h"

#include "ShaderLanguageTarget.h"
#include <string>
#include <vector>
#include <map>

namespace TrioEngine
{
	class EffectParameter;

	class TRIOAPI_DECL EffectParameterCollection
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
		std::map<std::string, BaseType> m_mData;
		std::vector<BaseType> m_vArray;
	};
}