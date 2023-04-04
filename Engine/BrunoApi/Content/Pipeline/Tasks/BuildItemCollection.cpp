#include "stdafx.h"
#include "BuildItemCollection.h"

#include "BuildItem.h"
#include "IO/File.h"

namespace BrunoEngine
{
	BuildItemCollection::BuildItemCollection()
	{
	}

	BuildItemCollection::~BuildItemCollection()
	{
	}

	void BuildItemCollection::Load(std::string filename)
	{
		if (BrunoIO::File::Exist(filename))
		{

		}
	}

	BuildItem* BuildItemCollection::FindItem(std::string outputFilename)
	{
		auto it = m_outputFilenameTable.find(outputFilename);
		if (it != m_outputFilenameTable.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	BuildItem* BuildItemCollection::FindItem(BuildRequest *request)
	{
		auto it = m_requestTable.find(request);
		if (it != m_requestTable.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	void BuildItemCollection::Clear()
	{
		m_items.clear();
		m_requestTable.clear();
		m_outputFilenameTable.clear();
		m_wantedItems.clear();
	}

	void BuildItemCollection::WantItem(BuildItem* item)
	{
		if (!item->m_isWanted)
		{
			m_wantedItems.push_back(item);
			item->m_isWanted = true;
		}
	}

	void BuildItemCollection::Remove(BuildItem* item)
	{
		std::vector<BuildItem*>::iterator it = find(m_items.begin(), m_items.end(), item);
		if (it != m_items.end())
		{
			m_items.erase(it);
		}
	}

	bool BuildItemCollection::RemoveUnwantedItems()
	{
		bool result = false;
		int i = 0;
		while (i < m_items.size())
		{
			if (m_items[i]->m_isWanted)
			{
				i++;
			}
			else
			{
				//base.RemoveAt(i);
				m_items.erase(m_items.begin() + i);
				result = true;
			}
		}
		return result;
	}

	void BuildItemCollection::PushBack(BuildItem *item)
	{
		if (m_requestTable.find(item->m_buildRequest) != m_requestTable.end())
		{
			throw std::exception();
		}
		if (m_outputFilenameTable.find(item->m_outputFilename) != m_outputFilenameTable.end())
		{
			throw std::exception();
		}
		m_items.push_back(item);
		m_requestTable[item->m_buildRequest] = item;
		m_outputFilenameTable[item->m_outputFilename] = item;
	}

	void BuildItemCollection::ReverseWantedItems(int fromIndex)
	{
		if (fromIndex < m_wantedItems.size())
		{
			std::reverse(m_wantedItems.begin() + fromIndex, m_wantedItems.begin() + m_wantedItems.size() - fromIndex);
		}
	}

	BuildItem* BuildItemCollection::PopWantedItem()
	{
		if (m_wantedItems.size() == 0)
			return nullptr;

		int index = m_wantedItems.size() - 1;
		BuildItem* item = m_wantedItems[index];
		m_wantedItems.erase(m_wantedItems.begin() + index);
		return item;
	}
}