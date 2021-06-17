#include "stdafx.h"
#include "GameServiceContainer.h"

namespace BrunoEngine
{
	GameServiceContainer::GameServiceContainer()
	{
	}


	GameServiceContainer::~GameServiceContainer()
	{
	}

	void GameServiceContainer::AddService(std::string typeName, void* object)
	{
		auto it = m_services.find(typeName);
		if (it != m_services.end())
		{
			throw std::exception();
		}
		m_services[typeName] = object;
	}

	IServiceBase* GameServiceContainer::GetService(std::string csTypeName)
	{
		auto it = m_services.find(csTypeName);
		if (it != m_services.end())
		{
			return (IServiceBase*)it->second;
		}
		return nullptr;
	}

	void GameServiceContainer::RemoveService(std::string csTypeName)
	{
		auto it = m_services.find(csTypeName);
		if (it == m_services.end())
		{
			throw std::exception();
		}
		m_services.erase(csTypeName);
	}
}
