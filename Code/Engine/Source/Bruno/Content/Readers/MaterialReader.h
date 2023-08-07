#pragma once

#include <Bruno/Content/ContentTypeReader.h>
#include <Bruno/Platform/DirectX/Texture.h>

namespace Bruno
{
	class MaterialReader : public ContentTypeReader<Texture>
	{
		BR_RTTI_DECLARATION(MaterialReader, AbstractContentTypeReader);

	public:
		MaterialReader();
		~MaterialReader() = default;

	protected:
		virtual std::shared_ptr<Texture> ReadInternal(ContentReader& input) override;
	};
}
