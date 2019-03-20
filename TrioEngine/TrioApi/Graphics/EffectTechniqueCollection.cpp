#include "stdafx.h"
#include "EffectTechniqueCollection.h"

#include "EffectTechnique.h"

namespace Vago
{
	EffectTechniqueCollection::EffectTechniqueCollection(std::vector<EffectTechnique*> parameters) :
		m_vArray(parameters)
	{
		for (size_t i = 0; i < m_vArray.size(); i++)
		{
			m_mData[m_vArray[i]->GetName()] = m_vArray[i];
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
		m_mData.clear();
		m_vArray.clear();
	}

	void EffectTechniqueCollection::push_back(EffectTechnique* technique)
	{
		if (technique == nullptr)
			return;

		m_vArray.push_back(technique);
		m_mData[technique->GetName()] = technique;
	}

	EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](std::string nName)
	{
		auto it = m_mData.find(nName);
		if (it == m_mData.end()) {
			m_mData[nName] = nullptr;
		}

		return m_mData[nName];
	}

	const EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](std::string nName) const
	{
		return m_mData.at(nName);
	}

	EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](int nIndex)
	{
		return m_vArray[nIndex];
	}

	const EffectTechniqueCollection::BaseType & EffectTechniqueCollection::operator[](int nIndex) const
	{
		return m_vArray[nIndex];
	}

	size_t EffectTechniqueCollection::size()
	{
		return m_vArray.size();
	}
}