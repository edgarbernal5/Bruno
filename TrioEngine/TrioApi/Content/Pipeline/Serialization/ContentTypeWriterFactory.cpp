#include "stdafx.h"
#include "ContentTypeWriterFactory.h"

#include "ContentTypeWriter.h"

#include "EffectWriter.h"
namespace TrioEngine
{
	ContentTypeWriterFactory::ContentTypeWriterFactory()
	{
		//RegisterContentTypeWriter<Texture2DWriter>();
		//RegisterContentTypeWriter<ModelWriter>();
		//RegisterContentTypeWriter<VertexBufferWriter>();
		//RegisterContentTypeWriter<IndexCollectionWriter>();
		//RegisterContentTypeWriter<MaterialWriter>();
		RegisterContentTypeWriter<EffectWriter>();
		//RegisterContentTypeWriter<EffectMaterialWriter>();
		//RegisterContentTypeWriter<ExternalReferenceWriter>();

		for (int i = 0; i < m_TypeWriters.size(); i++)
		{
			//m_TypeWriters[i]->Initialize(this);
		}
	}

	ContentTypeWriterFactory::~ContentTypeWriterFactory()
	{
	}

	ContentTypeWriter* ContentTypeWriterFactory::GetByWriterName(std::string const& writerName)
	{
		for (int i = 0; i < m_TypeWriters.size(); i++)
		{
			ContentTypeWriter* writer = m_TypeWriters[i];
			if (writer->GetWriterName() == writerName)
			{
				return writer;
			}
		}
		return nullptr;
	}
}