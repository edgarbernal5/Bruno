#pragma once

#include <Bruno/Content/ContentTypeReader.h>
#include <Bruno/Renderer/Model.h>

namespace Bruno
{
	class ModelReader : public ContentTypeReader<Model>
	{
		BR_RTTI_DECLARATION(ModelReader, AbstractContentTypeReader);

	public:
		ModelReader();
		~ModelReader() = default;

	protected:
		virtual std::shared_ptr<Model> ReadInternal(ContentReader& input) override;
	};
}
