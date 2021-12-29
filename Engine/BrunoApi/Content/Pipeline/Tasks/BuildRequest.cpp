#include "stdafx.h"
#include "BuildRequest.h"

namespace BrunoEngine
{
	BuildRequest::BuildRequest() : m_opaqueData(nullptr)
	{
	}

	BuildRequest::BuildRequest(const BuildRequest& other) {
		m_assetName = other.m_assetName;
		m_sourceFileName = other.m_sourceFileName;
		m_importerName = other.m_importerName;
		m_processorName = other.m_processorName;
		m_opaqueData = other.m_opaqueData;
	}

	BuildRequest::BuildRequest(const BuildRequest* & other) {
		m_assetName = other->m_assetName;
		m_sourceFileName = other->m_sourceFileName;
		m_importerName = other->m_importerName;
		m_processorName = other->m_processorName;
		m_opaqueData = other->m_opaqueData;
	}

	BuildRequest::~BuildRequest()
	{
		if (m_opaqueData)
		{
			delete m_opaqueData;
			m_opaqueData = nullptr;
		}
	}

}