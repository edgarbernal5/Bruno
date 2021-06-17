#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

namespace TrioEngine
{
	class BRUNO_API_EXPORT VertexChannelNames
	{
	public:
		static std::string Color(int usageIndex);
		static std::string Normal();
		static std::string Normal(int usageIndex);
		static std::string Weights(int usageIndex);
		static std::string TextureCoordinate(int usageIndex);

		static int DecodeUsageIndex(std::string name);
		static std::string DecodeBaseName(std::string& str);
		static VertexElementUsage TryDecodeUsage(const char* name);
	private:
		static std::string EncodeBase(const char* baseName, int usageIndex);
	};
}