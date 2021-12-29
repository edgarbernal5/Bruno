#pragma once

#include "TrioApiRequisites.h"

#include <string>

namespace BrunoEngine
{
	struct ContentIdentity
	{
		ContentIdentity() {}
		ContentIdentity(std::string sourceFilename)
		{
			m_sourceFilename = sourceFilename;
		}

		ContentIdentity(std::string sourceFilename, std::string sourceTool)
		{
			m_sourceFilename = sourceFilename;
			m_sourceTool = sourceTool;
		}

	private:
		std::string m_sourceFilename;
		std::string m_sourceTool;
	};

}