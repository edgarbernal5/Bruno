#pragma once

#include <Bruno/Content/ContentTypeReader.h>
#include <Bruno/Platform/DirectX/Texture.h>

namespace Bruno
{
	class ModelReader : public ContentTypeReader<Texture>
	{
		BR_RTTI_DECLARATION(ModelReader, AbstractContentTypeReader);

	public:
		ModelReader();
		~ModelReader() = default;

	protected:
		virtual std::shared_ptr<Texture> ReadInternal(ContentReader& input) override;
	};
}
