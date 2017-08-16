#include "stdafx.h"
#include "EffectParameterCollection.h"

#include "EffectParameter.h"

namespace Cuado
{

	EffectParameterCollection::EffectParameterCollection(std::vector<EffectParameter*> parameters) : 
		m_array(parameters)
	{
		for (size_t i = 0; i < m_array.size(); i++)
		{
			m_Data[m_array[i]->GetName()] = m_array[i];
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
		m_Data.clear();
		m_array.clear();
	}

	EffectParameterCollection::BaseType & EffectParameterCollection::operator[](std::string nName)
	{
		auto it = m_Data.find(nName);
		if (it == m_Data.end()) {
			m_Data[nName] = nullptr;
		}

		return m_Data[nName];
	}

	const EffectParameterCollection::BaseType & EffectParameterCollection::operator[](std::string nName) const
	{
		return m_Data.at(nName);
	}

	EffectParameterCollection::BaseType & EffectParameterCollection::operator[](int nIndex)
	{
		return m_array[nIndex];
	}

	const EffectParameterCollection::BaseType & EffectParameterCollection::operator[](int nIndex) const
	{
		return m_array[nIndex];
	}

	size_t EffectParameterCollection::size()
	{
		return m_array.size();
	}


	void EffectParameterCollection::push_back(EffectParameter* parameter)
	{
		if (parameter == nullptr)
			return;

		m_array.push_back(parameter);
		m_Data[parameter->GetName()] = parameter;
	}
}