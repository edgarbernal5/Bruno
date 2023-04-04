#pragma once

#include "TrioApiRequisites.h"

namespace BrunoEngine
{
	class BRUNO_API_EXPORT RenderPath
	{
	public:
		virtual void Render() /*const*/ = 0;

	protected:
		virtual void RenderShadows() /*const*/ = 0;
	};
}