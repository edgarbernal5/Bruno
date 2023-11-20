#pragma once

#include "Bruno/Content/ContentTypeReader.h"
#include "Bruno/Platform/DirectX/Texture.h"

namespace Bruno
{
	class TextureReader : public ContentTypeReader<Texture>
	{
		BR_RTTI_DECLARATION(TextureReader, AbstractContentTypeReader);

	public:
		TextureReader();
		~TextureReader() = default;

	protected:
		virtual std::shared_ptr<Texture> ReadInternal(ContentReader& input) override;
	};
}
