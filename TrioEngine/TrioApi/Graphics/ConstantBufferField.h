#pragma once

#include "TrioApiDecl.h"

#include <string>

namespace Vago
{
	struct TRIOAPI_DECL ConstantBufferField
	{
		std::string Name;
		uint32_t Offset;
		uint32_t SizeInBytes;
		std::string BaseTypeName;

		ConstantBufferField() {}
		ConstantBufferField(std::string _name, uint32_t _offset, uint32_t _size, std::string _baseName) :
			Name(_name), Offset(_offset), SizeInBytes(_size), BaseTypeName(_baseName) {}
	};
}
