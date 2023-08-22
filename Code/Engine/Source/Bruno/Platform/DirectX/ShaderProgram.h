#pragma once

#include "D3DCommon.h"
#include "Bruno/Core/RTTI.h"

namespace Bruno
{
	class ShaderProgram : public RTTI
	{
		BR_RTTI_DECLARATION(ShaderProgram, RTTI);

	public:
		ShaderProgram(const std::wstring& sourceFilename, const std::string& entryPoint, const  std::string& target);
		ShaderProgram(const std::vector<uint8_t>& compiledCode);

		ID3DBlob* GetBlob() { return m_blob.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	};
}

