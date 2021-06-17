#include "stdafx.h"
#include "BuildItem.h"


namespace TrioEngine
{
	BuildItem::BuildItem() :
		m_isBuilt(false), m_isWanted(false), m_buildRequest(nullptr)
	{
	}

	bool BuildItem::ContainsDependency(std::string filename)
	{
		auto it = m_dependencies.find(filename);
		return it != m_dependencies.end();
	}

	bool BuildItem::ContainsRequest(std::string filename)
	{
		auto it = std::find(m_requests.begin(), m_requests.end(), filename);
		return it != m_requests.end();
	}

	void BuildItem::RemoveDependency(std::string filename)
	{
		m_dependencies.erase(filename);
	}

	void BuildItem::RemoveRequest(std::string filename)
	{
		auto it = std::find(m_requests.begin(), m_requests.end(), filename);
		if (it != m_requests.end())
			m_requests.erase(it);
	}
}