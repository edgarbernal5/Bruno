#pragma once

#include <Bruno/Content/ContentTypeReader.h>
#include <Bruno/Renderer/Material.h>

namespace Bruno
{
	class MaterialReader : public ContentTypeReader<Material>
	{
		BR_RTTI_DECLARATION(MaterialReader, AbstractContentTypeReader);

	public:
		MaterialReader();
		~MaterialReader() = default;

	protected:
		virtual std::shared_ptr<Material> ReadInternal(ContentReader& input) override;
	};
}
