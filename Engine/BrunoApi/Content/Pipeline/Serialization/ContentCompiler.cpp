#include "stdafx.h"
#include "ContentCompiler.h"

#include "ContentTypeWriter.h"
#include "ContentTypeWriterFactory.h"
#include "ContentWriter.h"

namespace BrunoEngine
{
	ContentCompiler::ContentCompiler() : m_factory(nullptr)
	{
		m_factory = new ContentTypeWriterFactory();
	}

	ContentCompiler::~ContentCompiler()
	{
		if (m_factory)
		{
			delete m_factory;
			m_factory = nullptr;
		}
	}

	ContentTypeWriter* ContentCompiler::GetTypeWriter(std::string name, std::vector<std::string> &dependencies)
	{
		ContentTypeWriter* writer = GetTypeWriterInternal(name);
		auto itm = m_writerDependencies.find(writer);
		dependencies = (*itm).second;
		return writer;
	}

	void ContentCompiler::AddTypeWriter(ContentTypeWriter* writer)
	{
		std::string name = writer->GetWriterName();
		m_writerInstances[name] = writer;
		m_writerDependencies[writer] = std::vector<std::string>();
	}

	ContentTypeWriter* ContentCompiler::GetTypeWriterInternal(std::string name)
	{
		auto itm = m_writerInstances.find(name);
		if (itm != m_writerInstances.end())
		{
			return (*itm).second;
		}

		ContentTypeWriter* writer = m_factory->GetByWriterName(name);
		AddTypeWriter(writer);
		//InitializeTypeWriter
		return writer;
	}

	void ContentCompiler::Compile(TrioIO::Stream *stream, ContentItem *object, bool compressContent, std::string rootDirectory, std::string referenceRelocationPath)
	{
		ContentWriter *writer = new ContentWriter(this, stream, compressContent, rootDirectory, referenceRelocationPath);

		writer->WriteObject(object);
		writer->FlushOutput();

		delete writer;
	}
}