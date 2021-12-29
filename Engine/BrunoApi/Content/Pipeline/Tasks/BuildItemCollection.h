#pragma once

#include "TrioApiRequisites.h"

#include "BuildRequest.h"
namespace BrunoEngine
{
	class BuildItem;
	class BuildRequest;



	struct Compare_P {
		bool operator()(BuildRequest const* a, BuildRequest const* b)const {
			return a->m_sourceFileName < b->m_sourceFileName;
		}
	};

	class BRUNO_API_EXPORT BuildItemCollection
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
			return m_items[nIndex];
		}

		BaseType & operator[](int nIndex)
		{
			return m_items[nIndex];
		}

		inline std::vector<BuildItem*>::size_type Size()
		{
			return m_items.size();
		}

		inline int WantedItemsCount()
		{
			return m_wantedItems.size();
		}

		void Clear();
		void PushBack(BuildItem *item);

		void Load(std::string filename);
		BuildItem* FindItem(std::string outputFilename);
		BuildItem* FindItem(BuildRequest *request);

		void Remove(BuildItem* item);
		bool RemoveUnwantedItems();
		void WantItem(BuildItem* item);
		void ReverseWantedItems(int fromIndex);
		BuildItem* PopWantedItem();
		friend class BuildCoordinator;


	private:
		//struct ptr_less {
		//	bool operator()(const BuildRequest* lhs, const BuildRequest* rhs) {
		//		return lhs->m_sourceFileName < rhs->m_sourceFileName;
		//	}
		//};

		std::vector<BuildItem*> m_items;
		std::vector<BuildItem*> m_wantedItems;

		std::vector<std::string> m_previousOutputs;

		std::map<BuildRequest*, BuildItem*, Compare_P> m_requestTable;
		std::map<std::string, BuildItem*> m_outputFilenameTable;


	};

}