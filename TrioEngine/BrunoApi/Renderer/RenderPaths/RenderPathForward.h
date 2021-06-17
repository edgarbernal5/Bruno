#pragma once

#include "TrioApiRequisites.h"
#include "RenderPath.h"

namespace TrioEngine
{
	class EffectPass;
	class EffectParameter;

	class BRUNO_API_EXPORT RenderPathForward : public RenderPath
	{
	public:
		//RenderPathForward() = default;
		RenderPathForward();

		void Render() const override;

	protected:
		void RenderShadows() const override;

	private:
		EffectPass* m_mainPass;
		EffectParameter* m_mainParameter;
	};
}