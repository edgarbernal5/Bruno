#include "ContentCompiler.h"
#include "ContentWriter.h"

namespace Bruno
{
	void ContentCompiler::Compile(Stream& stream, const ContentItem& object, bool compressContent, const std::wstring& rootDirectory, const std::wstring& referenceRelocationPath)
	{
		ContentWriter writer(this, stream, compressContent, rootDirectory, referenceRelocationPath);
		writer.WriteObject(object);
		writer.FlushOutput();
	}
}