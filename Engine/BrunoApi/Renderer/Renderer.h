#pragma once

#include "TrioApiRequisites.h"
#include "Renderer\Camera.h"
#include "FrameCulling.h"

#include <unordered_map>

namespace BrunoEngine
{
	//enum class RenderPassFlags
	//{
	//	Texture,
	//	Deferred,
	//	Forward,
	//	TiledForward
	//};

	class GraphicsDevice;
	class EffectParameter;

	class BRUNO_API_EXPORT Renderer
	{
	public:
		static void DrawScene(EffectParameter* mvpParameter, EffectParameter* modelParameter, Matrix& viewProjection /*const Camera& camera*/);
		static GraphicsDevice* GetDevice();
		static void Initialize(GraphicsDevice* device);
		static void UpdatePerFrameData();
		
	private:
		static GraphicsDevice* g_device;
		static std::unordered_map<const Camera*, FrameCulling> g_frameCullings;

		static void PerformFrameCulling();
	};
}