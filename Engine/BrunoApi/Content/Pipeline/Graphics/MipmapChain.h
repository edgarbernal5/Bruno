#pragma once

#include "TrioApiRequisites.h"

#include <vector>

namespace BrunoEngine
{
	class BitmapContent;

	class MipmapChain : public std::vector<BitmapContent*>
	{
	public:
		MipmapChain()
		{
		}

		MipmapChain(BitmapContent* bitmap)
		{
			push_back(bitmap);
		}

		~MipmapChain()
		{
		}
	};

	class MipmapChainCollection : public std::vector<MipmapChain>
	{
	public:
		MipmapChainCollection()
		{
		}

		MipmapChainCollection(int initialSize)
		{
			for (int i = 0; i < initialSize; i++)
			{
				push_back(MipmapChain());
			}
		}

		~MipmapChainCollection()
		{
		}
	};
}