#include "stdafx.h"
#include "BuildRequest.h"

namespace TrioEngine
{
	BuildRequest::BuildRequest() : m_OpaqueData(nullptr)
	{

	}
	BuildRequest::~BuildRequest()
	{
		if (m_OpaqueData)
		{
			delete m_OpaqueData;
			m_OpaqueData = nullptr;
		}
	}
}