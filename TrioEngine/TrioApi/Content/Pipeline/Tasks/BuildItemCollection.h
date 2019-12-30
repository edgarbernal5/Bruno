#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	class BuildItem;
	class BuildRequest;

	class TRIO_API_EXPORT BuildItemCollection
	{
		//typedef vector<BuildItem*> vectorB;
	public:
		BuildItemCollection();
		~BuildItemCollection();

		//using vectorB::push_back;
		//using vectorB::operator[];
		//using vectorB::begin;
		//using vectorB::end;
		typedef BuildItem* BaseType;

		const BaseType & operator[](int nIndex) const
		{
			return m_Items[nIndex];
		}

		BaseType & operator[](int nIndex)
		{
			return m_Items[nIndex];
		}

		inline std::vector<BuildItem*>::size_type Size()
		{
			return m_Items.size();
		}

		inline int WantedItemsCount()
		{
			return m_WantedItems.size();
		}

		void Clear();
		void PushBack(BuildItem *item);

		void Load(std::string filename);
		BuildItem* FindItem(std::string outputFilename);
		BuildItem* FindItem(BuildRequest *request);

		void Remove(BuildItem* item);
		void WantItem(BuildItem* item);
		void ReverseWantedItems(int fromIndex);
		BuildItem* PopWantedItem();
		friend class BuildCoordinator;
	private:
		std::vector<BuildItem*> m_Items;
		std::vector<BuildItem*> m_WantedItems;

		std::vector<std::string> m_PreviousOutputs;

		std::map<BuildRequest*, BuildItem*> m_RequestTable;
		std::map<std::string, BuildItem*> m_OutputFilenameTable;
	};
}