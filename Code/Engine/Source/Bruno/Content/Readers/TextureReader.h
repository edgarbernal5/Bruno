#pragma once

#include "Bruno/Content/ContentTypeReader.h"
#include "Bruno/Platform/DirectX/Texture2D.h"

namespace Bruno
{
	class TextureReader : public ContentTypeReader<Texture2D>
	{
		BR_RTTI_DECLARATION(TextureReader, AbstractContentTypeReader);

	public:
		TextureReader();
		~TextureReader() = default;

	protected:
		virtual std::shared_ptr<Texture2D> ReadInternal(ContentReader& input) override;
	};
}
