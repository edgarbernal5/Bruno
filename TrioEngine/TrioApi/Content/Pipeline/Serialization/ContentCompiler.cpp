#include "stdafx.h"
#include "ContentCompiler.h"

#include "ContentTypeWriter.h"
#include "ContentTypeWriterFactory.h"
#include "ContentWriter.h"

namespace TrioEngine
{
	ContentCompiler::ContentCompiler() : m_Factory(nullptr)
	{
		m_Factory = new ContentTypeWriterFactory();
	}

	ContentCompiler::~ContentCompiler()
	{
	}


	ContentTypeWriter* ContentCompiler::GetTypeWriter(std::string name, std::vector<std::string> &dependencies)
	{
		ContentTypeWriter* write = GetTypeWriterInternal(name);
		std::map<ContentTypeWriter*, std::vector<std::string> >::iterator itm = m_WriterDependencies.find(write);
		dependencies = (*itm).second;
		return write;
	}

	void ContentCompiler::AddTypeWriter(ContentTypeWriter* writer)
	{
		std::string name = writer->GetWriterName();
		m_WriterInstances[name] = writer;
		m_WriterDependencies[writer] = std::vector<std::string>();
	}

	ContentTypeWriter* ContentCompiler::GetTypeWriterInternal(std::string name)
	{
		std::map<std::string, ContentTypeWriter*>::iterator itm = m_WriterInstances.find(name);
		ContentTypeWriter* writer = nullptr;
		if (itm != m_WriterInstances.end())
		{
			return (*itm).second;
		}

		writer = m_Factory->GetByWriterName(name);
		AddTypeWriter(writer);
		//InitializeTypeWriter
		return writer;
	}

	void ContentCompiler::Compile(TrioIO::Stream *stream, ContentItem *object, bool compressContent)
	{
		ContentWriter *writer = new ContentWriter(this, stream, compressContent);

		writer->WriteObject(object);
		writer->FlushOutput();

		delete writer;
	}
}