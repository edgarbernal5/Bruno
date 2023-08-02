#pragma once

#include "D3DCommon.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class Shader : public RTTI
	{
		BR_RTTI_DECLARATION(Shader, RTTI);

	public:
		Shader(const std::wstring& pathToSource, std::string entryPoint, std::string target);

		ID3DBlob* GetBlob() { return m_blob.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	};


}

