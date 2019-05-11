#include "stdafx.h"
#include "EffectParameterCollection.h"

#include "EffectParameter.h"

namespace TrioEngine
{
	EffectParameterCollection::EffectParameterCollection(std::vector<EffectParameter*> parameters) :
		m_vArray(parameters)
	{
		for (size_t i = 0; i < m_vArray.size(); i++)
		{
			m_mData[m_vArray[i]->GetName()] = m_vArray[i];
		}
	}

	EffectParameterCollection::EffectParameterCollection()
	{
	}

	EffectParameterCollection::~EffectParameterCollection()
	{
	}

	void EffectParameterCollection::clear()
	{
		m_mData.clear();
		m_vArray.clear();
	}

	EffectParameterCollection::BaseType & EffectParameterCollection::operator[](std::string nName)
	{
		auto it = m_mData.find(nName);
		if (it == m_mData.end()) {
			m_mData[nName] = nullptr;
		}

		return m_mData[nName];
	}

	const EffectParameterCollection::BaseType & EffectParameterCollection::operator[](std::string nName) const
	{
		return m_mData.at(nName);
	}

	EffectParameterCollection::BaseType & EffectParameterCollection::operator[](int nIndex)
	{
		return m_vArray[nIndex];
	}

	const EffectParameterCollection::BaseType & EffectParameterCollection::operator[](int nIndex) const
	{
		return m_vArray[nIndex];
	}

	size_t EffectParameterCollection::size()
	{
		return m_vArray.size();
	}

	void EffectParameterCollection::push_back(EffectParameter* parameter)
	{
		if (parameter == nullptr)
			return;

		m_vArray.push_back(parameter);
		m_mData[parameter->GetName()] = parameter;
	}
}