#pragma once

#include "Bruno/Content/ContentTypeReader.h"
#include "Bruno/Platform/DirectX/Shader.h"

namespace Bruno
{
	class ShaderReader : public ContentTypeReader<Shader>
	{
		BR_RTTI_DECLARATION(ShaderReader, AbstractContentTypeReader);

	public:
		ShaderReader();
		~ShaderReader() = default;

	protected:
		virtual std::shared_ptr<Shader> ReadInternal(ContentReader& input) override;
	};
}

