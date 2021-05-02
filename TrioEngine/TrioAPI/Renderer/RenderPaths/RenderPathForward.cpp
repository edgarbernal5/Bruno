#include "stdafx.h"
#include "RenderPathForward.h"

#include "Renderer/Renderer.h"
#include "Scene.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/Effect.h"
#include "Graphics/EffectLoader.h"

#include "Math/MathVector.h"

namespace TrioEngine
{
	RenderPathForward::RenderPathForward()
	{
		Effect* effect = new Effect(Renderer::GetDevice());
		EffectLoader effectLoader(effect);

		effectLoader.LoadFromFile("D:/Edgar/Documentos/Proyectos/CG/TrioEngineGit/Shaders/SceneTest.fx");

		m_mainParameter = effect->GetParameters()[0];
		m_mainPass = effect->GetTechniques()[0]->GetPasses()[0];
	}

	void RenderPathForward::Render() const
	{
		Scene& scene = *Scene::GetActiveScene();
		Camera& camera = *Scene::GetCamera();

		Matrix viewMatrix = Matrix::CreateLookAt(camera.GetPosition(), camera.GetTarget(), camera.GetUp());
		Matrix projectionMatrix = Matrix::CreatePerspectiveFieldOfView(camera.GetFieldOfView(), camera.GetAspectRatio(), camera.GetNearPlane(), camera.GetFarPlane());
		Matrix viewProjection = viewMatrix * projectionMatrix;

		m_mainParameter->SetValue(viewProjection);
		m_mainPass->Apply();
		Renderer::DrawScene();
	}

	void RenderPathForward::RenderShadows() const
	{

	}
}