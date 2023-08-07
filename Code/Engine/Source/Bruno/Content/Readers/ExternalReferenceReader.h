#pragma once

#include <Bruno/Content/ContentTypeReader.h>

namespace Bruno
{
	class ExternalReferenceReader : public AbstractContentTypeReader
	{
		BR_RTTI_DECLARATION(ExternalReferenceReader, AbstractContentTypeReader);

	public:
		ExternalReferenceReader();
		~ExternalReferenceReader() = default;

		virtual std::shared_ptr<RTTI> Read(ContentReader& input) override;
	};
}
