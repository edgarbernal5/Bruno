#pragma once

#include "TrioAPI.h"

#include <string>
#include "VertexElement.h"


namespace Cuado
{
	class TRIOAPI_DLL VertexChannelNames
	{
	public:
		static std::string Normal();
		static std::string Normal(int usageIndex);
		static std::string Weights(int usageIndex);
		static std::string TextureCoordinate(int usageIndex);

		static int DecodeUsageIndex(std::string name);
		static std::string DecodeBaseName(std::string str);
		static VertexElementUsage TryDecodeUsage(std::string name);
	private:
		static std::string EncodeBase(std::string baseName, int usageIndex);
	};

}
