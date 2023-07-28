#pragma once

#include <string>

namespace Bruno
{
	struct GameContentBuilderSettings
	{
		std::string IntermediateDirectory;
		std::string OutputDirectory;
		std::string RootDirectory;
	};

	class GameContentBuilder
	{
	public:
		GameContentBuilder(const GameContentBuilderSettings& settings);

	private:
		GameContentBuilderSettings m_settings;
	};
}

