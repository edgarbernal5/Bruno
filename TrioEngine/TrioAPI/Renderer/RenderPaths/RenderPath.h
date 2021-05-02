#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	class TRIO_API_EXPORT RenderPath
	{
	public:
		virtual void Render() const = 0;

	protected:
		virtual void RenderShadows() const = 0;
	};
}