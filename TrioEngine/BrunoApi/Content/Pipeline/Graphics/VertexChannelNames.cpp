#include "stdafx.h"
#include "VertexChannelNames.h"
#include <sstream>

namespace TrioEngine
{
	VertexElementUsage VertexChannelNames::TryDecodeUsage(const char* name)
	{
		std::string copyName = name;
		for (int i = copyName.size() - 1; i >= 0; i--)
		{
			if (copyName[i] >= '0' && copyName[i] <= '9')
				copyName.erase(copyName.begin() + i);
			else
				break;
		}
		if (copyName == "Normal")
		{
			return VertexElementUsage::Normal;
		}
		if (copyName == "TextureCoordinate")
		{
			return VertexElementUsage::TextureCoordinate;
		}
		if (copyName == "Position")
		{
			return VertexElementUsage::Position;
		}
		if (copyName == "Color")
		{
			return VertexElementUsage::Color;
		}
		if (copyName == "Binormal")
		{
			return VertexElementUsage::Binormal;
		}
		if (copyName == "Tangent")
		{
			return VertexElementUsage::Tangent;
		}
		return VertexElementUsage::Position;
	}

	std::string VertexChannelNames::Color(int usageIndex)
	{
		return EncodeBase("Color", usageIndex);
	}

	std::string VertexChannelNames::Normal()
	{
		return Normal(0);
	}

	int VertexChannelNames::DecodeUsageIndex(std::string name)
	{
		if (name.size() == 0)
			return -1;

		int length = DecodeBaseName(name).size();
		std::string snum = name.substr(length);

		if (snum.size() == 0)
			return 0;

		int numb;
		std::istringstream (snum) >> numb;
		return numb;
	}

	std::string VertexChannelNames::DecodeBaseName(std::string& str)
	{
		int num = str.size() - 1;
		int startIndex = 0;
		std::string trimChars = "0123456789";
		while (num >= startIndex)
		{
			int num4 = 0;
			char ch2 = str[num];
			num4 = 0;
			while (num4 < trimChars.size())
			{
				if (trimChars[num4] == ch2)
				{
					break;
				}
				num4++;
			}
			if (num4 == trimChars.size())
			{
				break;
			}
			num--;
		}
		int length = (num - startIndex) + 1;
		if (length == str.size())
		{
			return str;
		}
		if (length == 0)
		{
			return "";
		}
		return str.substr(startIndex, length);
	}

	std::string VertexChannelNames::Normal(int usageIndex)
	{
		return EncodeBase("Normal", usageIndex);
	}


	std::string VertexChannelNames::Weights(int usageIndex)
	{
		return EncodeBase("Weights", usageIndex);
	}

	std::string VertexChannelNames::TextureCoordinate(int usageIndex)
	{
		return EncodeBase("TextureCoordinate", usageIndex);
	}

	std::string VertexChannelNames::EncodeBase(const char* baseName, int usageIndex)
	{
		std::stringstream ss;
		ss << baseName << usageIndex;
		//sprintf(buff, "%s%d", baseName.c_str(), usageIndex);
		return ss.str();
	}
}