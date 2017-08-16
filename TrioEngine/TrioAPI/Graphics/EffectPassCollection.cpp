#include "stdafx.h"
#include "EffectPassCollection.h"

#include "EffectPass.h"

namespace Cuado
{

	EffectPassCollection::EffectPassCollection(std::vector<EffectPass*> parameters) : m_array(parameters)
	{
		for (size_t i = 0; i < m_array.size(); i++)
		{
			m_Data[m_array[i]->GetName()] = m_array[i];
		}
	}

	EffectPassCollection::EffectPassCollection()
	{
	}

	EffectPassCollection::~EffectPassCollection()
	{
	}

	void EffectPassCollection::clear()
	{
		m_Data.clear();
		m_array.clear();
	}

	EffectPassCollection::BaseType & EffectPassCollection::operator[](std::string nName)
	{
		auto it = m_Data.find(nName);
		if (it == m_Data.end()) {
			m_Data[nName] = nullptr;
		}

		return m_Data[nName];
	}

	const EffectPassCollection::BaseType & EffectPassCollection::operator[](std::string nName) const
	{
		return m_Data.at(nName);
	}

	EffectPassCollection::BaseType & EffectPassCollection::operator[](int nIndex)
	{
		return m_array[nIndex];
	}

	const EffectPassCollection::BaseType & EffectPassCollection::operator[](int nIndex) const
	{
		return m_array[nIndex];
	}

	size_t EffectPassCollection::size()
	{
		return m_array.size();
	}
}