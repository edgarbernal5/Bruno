#include "stdafx.h"
#include "IndexCollectionWriter.h"

#include "ContentWriter.h"
#include "Content/Pipeline/Graphics/IndexCollection.h"

namespace BrunoEngine
{
	IndexCollectionWriter::IndexCollectionWriter()
	{
	}

	IndexCollectionWriter::~IndexCollectionWriter()
	{
	}

	void IndexCollectionWriter::Write(ContentWriter *output, ContentItem* value)
	{
		IndexCollection* content = (IndexCollection*)value;

		bool flag = true;
		for (int i = 0; i < content->size(); i++)
		{
			if ((*content)[i] > 0xfffe)
			{
				flag = false;
				break;
			}
		}

		int num2 = content->size() * (flag ? 2 : 4);

		output->WriteBool(flag);
		output->WriteInt32(num2);
		for (int i = 0; i < content->size(); i++)
		{
			int num = (*content)[i];
			output->WriteByte((num & 0xff));
			output->WriteByte(((num >> 8) & 0xff));
			if (!flag)
			{
				output->WriteByte(((num >> 0x10) & 0xff));
				output->WriteByte(((num >> 0x18) & 0xff));
			}
		}
	}
}