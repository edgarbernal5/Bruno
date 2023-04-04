#pragma once

#include "TrioApiRequisites.h"
#include "RenderPath.h"

#include "Math/MathVector.h"

namespace BrunoEngine
{
	class EffectPass;
	class EffectParameter;
	class ConstantBuffer;

	class BRUNO_API_EXPORT RenderPathForward : public RenderPath
	{
	public:
		//RenderPathForward() = default;
		RenderPathForward();

		void Render() /*const*/ override;

	protected:
		void RenderShadows() /*const*/ override;

	private:
		EffectPass* m_mainPass;
		EffectParameter* m_mvpParameter;
		EffectParameter* m_worldParameter;

		ConstantBuffer* m_lightBuffer;

		struct LightBuffer {

			Vector4 ambientColor { 0.05f, 0.05f, 0.05f, 1.0f };
			Vector3 lightDirection { 0.5f, 0.5f, 0 };
			float padding;
			Vector3 lightColor {1, 1, 1 };

			LightBuffer() = default;
			LightBuffer(const LightBuffer& copy) = default;
			LightBuffer& operator= (const LightBuffer& other) = default;
		};

		LightBuffer m_lightData;

		void UpdateLightProperties();
	};
}