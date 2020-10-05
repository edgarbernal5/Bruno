#pragma once

#include "TrioApiRequisites.h"

#include <exception>
#include <Windows.h>
#include <string>

namespace TrioEngine
{
	class GameException : public std::exception
	{
	public:
		GameException(const char* const& message, HRESULT hr = S_OK);

		HRESULT HR() const;
		std::wstring whatw() const;

	private:
		HRESULT mHR;
	};
}