#include "stdafx.h"
#include "GameServiceContainer.h"

namespace TrioEngine
{
	GameServiceContainer::GameServiceContainer()
	{
	}


	GameServiceContainer::~GameServiceContainer()
	{
	}

	void GameServiceContainer::AddService(std::string typeName, void* object)
	{
		auto it = m_mServices.find(typeName);
		if (it != m_mServices.end())
		{
			throw std::exception();
		}
		m_mServices[typeName] = object;
	}

	IServiceBase* GameServiceContainer::GetService(std::string csTypeName)
	{
		auto it = m_mServices.find(csTypeName);
		if (it != m_mServices.end())
		{
			return (IServiceBase*)it->second;
		}
		return nullptr;
	}

	void GameServiceContainer::RemoveService(std::string csTypeName)
	{
		auto it = m_mServices.find(csTypeName);
		if (it == m_mServices.end())
		{
			throw std::exception();
		}
		m_mServices.erase(csTypeName);
	}
}
