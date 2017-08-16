#include "stdafx.h"
#include "GameServiceContainer.h"


namespace Cuado
{
	GameServiceContainer::GameServiceContainer()
	{
	}


	GameServiceContainer::~GameServiceContainer()
	{
	}

	IServiceBase* GameServiceContainer::GetService(string typeName)
	{
		auto it = m_services.find(typeName);
		if (it != m_services.end())
		{
			return (IServiceBase*)it->second;
		}
		return nullptr;
	}

	void GameServiceContainer::AddService(string typeName, void* object)
	{
		auto it = m_services.find(typeName);
		if (it != m_services.end())
		{
			throw std::exception();
		}
		m_services[typeName] = object;
	}

	void GameServiceContainer::RemoveService(string typeName)
	{
		auto it = m_services.find(typeName);
		if (it == m_services.end())
		{
			throw std::exception();
		}
		m_services.erase(typeName);
	}
}