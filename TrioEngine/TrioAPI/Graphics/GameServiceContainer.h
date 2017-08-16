#pragma once
#include "TrioAPI.h"

#include <typeinfo>
#include <map>
#include <string>
#include "IServiceProvider.h"

using namespace std;

namespace Cuado
{
	
	class TRIOAPI_DLL GameServiceContainer : public IServiceProvider
	{
	public:
		GameServiceContainer();
		~GameServiceContainer();

		void AddService(string typeName, void* object);
		void RemoveService(string typeName);
		IServiceBase* GetService(string typeName);

	private:
		map<string, void*> m_services;
	};

}
