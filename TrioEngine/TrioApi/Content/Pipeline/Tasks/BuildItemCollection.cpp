#include "stdafx.h"
#include "BuildItemCollection.h"

#include "BuildItem.h"

namespace TrioEngine
{
	BuildItemCollection::BuildItemCollection()
	{
	}

	BuildItemCollection::~BuildItemCollection()
	{
	}

	void BuildItemCollection::Load(std::string filename)
	{

	}

	BuildItem* BuildItemCollection::FindItem(std::string outputFilename)
	{
		std::map<std::string, BuildItem*>::iterator it = m_OutputFilenameTable.find(outputFilename);
		if (it != m_OutputFilenameTable.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	BuildItem* BuildItemCollection::FindItem(BuildRequest *request)
	{
		std::map<BuildRequest*, BuildItem*>::iterator it = m_RequestTable.find(request);
		if (it != m_RequestTable.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	void BuildItemCollection::Clear()
	{
		m_Items.clear();
		m_RequestTable.clear();
		m_OutputFilenameTable.clear();
		m_OutputFilenameTable.clear();
	}

	void BuildItemCollection::WantItem(BuildItem* item)
	{
		if (!item->m_IsWanted)
		{
			m_WantedItems.push_back(item);
			item->m_IsWanted = true;
		}
	}

	void BuildItemCollection::Remove(BuildItem* item)
	{
		std::vector<BuildItem*>::iterator it = find(m_Items.begin(), m_Items.end(), item);
		if (it != m_Items.end())
		{
			m_Items.erase(it);
		}
	}

	void BuildItemCollection::PushBack(BuildItem *item)
	{
		if (m_RequestTable.find(item->m_BuildRequest) != m_RequestTable.end())
		{
			throw std::exception();
		}
		if (m_OutputFilenameTable.find(item->OutputFilename) != m_OutputFilenameTable.end())
		{
			throw std::exception();
		}
		m_Items.push_back(item);
		m_RequestTable[item->m_BuildRequest] = item;
		m_OutputFilenameTable[item->OutputFilename] = item;
	}

	void BuildItemCollection::ReverseWantedItems(int fromIndex)
	{
		if (fromIndex < m_WantedItems.size())
		{
			std::reverse(m_WantedItems.begin() + fromIndex, m_WantedItems.begin() + m_WantedItems.size() - fromIndex);
		}
	}

	BuildItem* BuildItemCollection::PopWantedItem()
	{
		if (m_WantedItems.size() == 0)
			return nullptr;

		int index = m_WantedItems.size() - 1;
		BuildItem* item = m_WantedItems[index];
		m_WantedItems.erase(m_WantedItems.begin() + index);
		return item;
	}
}