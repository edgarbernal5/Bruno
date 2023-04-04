#include "stdafx.h"
#include "RenderPathForward.h"

#include "Renderer/Renderer.h"
#include "Scene.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/Effect.h"
#include "Graphics/EffectLoader.h"
#include "Graphics/ConstantBuffer.h"


namespace BrunoEngine
{
	RenderPathForward::RenderPathForward()
	{
		Effect* effect = new Effect(Renderer::GetDevice());
		EffectLoader effectLoader(effect);

		effectLoader.LoadFromFile("D:/Edgar/Documentos/Proyectos/CG/BrunoEngineGit/Shaders/ForwardRendering/Opaque.fx");

		m_mvpParameter = effect->GetParameters()["gWorldViewProj"];
		m_worldParameter = effect->GetParameters()["gWorld"];
		m_mainPass = effect->GetTechniques()[0]->GetPasses()[0];

		m_lightBuffer = effect->GetConstantBuffers()[1];
	}

	void RenderPathForward::Render()
	{
		Scene& scene = *Scene::GetActiveScene();
		Camera& camera = *Scene::GetCamera();

		Matrix viewMatrix = Matrix::CreateLookAt(camera.GetPosition(), camera.GetTarget(), camera.GetUp());
		Matrix projectionMatrix = Matrix::CreatePerspectiveFieldOfView(camera.GetFieldOfView(), camera.GetAspectRatio(), camera.GetNearPlane(), camera.GetFarPlane());
		Matrix viewProjection = viewMatrix * projectionMatrix;
		
		UpdateLightProperties();
		m_mainPass->Apply();
		Renderer::DrawScene(m_mvpParameter, m_worldParameter, viewProjection);


	}

	void RenderPathForward::UpdateLightProperties()
	{
		Scene& scene = *Scene::GetActiveScene();
		auto& lights = scene.GetLights();
		if (lights.GetCount() > 0) {
			m_lightData.lightDirection = lights[0].m_lightDirection;

			m_lightBuffer->SetData(m_lightData);
		}

	}

	void RenderPathForward::RenderShadows()
	{

	}
}