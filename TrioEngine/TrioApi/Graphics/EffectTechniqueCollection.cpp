#include "stdafx.h"
#include "EffectTechniqueCollection.h"

#include "EffectTechnique.h"

namespace TrioEngine
{
	EffectTechniqueCollection::EffectTechniqueCollection(std::vector<EffectTechnique*> parameters) :
		m_array(parameters)
	{
		for (size_t i = 0; i < m_array.size(); i++)
		{
			m_data[m_array[i]->GetName()] = m_array[i];
		}
	}

	EffectTechniqueCollection::EffectTechniqueCollection()
	{
	}

	EffectTechniqueCollection::~EffectTechniqueCollection()
	{
	}

	void EffectTechniqueCollection::clear()
	{
		m_data.clear();
		m_array.clear();
	}

	void EffectTechniqueCollection::push_back(EffectTechnique* technique)
	{
		if (technique == nullptr)
			return;

		m_array.push_back(technique);
		m_data[technique->GetName()] = technique;
	}

	EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](std::string nName)
	{
		auto it = m_data.find(nName);
		if (it == m_data.end()) {
			m_data[nName] = nullptr;
		}

		return m_data[nName];
	}

	const EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](std::string nName) const
	{
		return m_data.at(nName);
	}

	EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](int nIndex)
	{
		return m_array[nIndex];
	}

	const EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](int nIndex) const
	{
		return m_array[nIndex];
	}

	size_t EffectTechniqueCollection::size()
	{
		return m_array.size();
	}
}