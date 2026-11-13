#include "brpch.h"
#include "ShadowSystem.h"

#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    ShadowSystem::ShadowSystem(Camera& camera, std::shared_ptr<Scene> scene) : 
        m_camera(camera),
        m_scene(scene)
    {
    }

    void ShadowSystem::Execute(uint32_t resolution, uint32_t numCascades, float lambda)
    {
        m_shadowMapResolution = resolution;
        m_numCascades = numCascades;
        m_cascadeLambda = lambda;
        
        auto lightView = m_scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
        
        Math::Vector3 directionalLightDir;
        for (auto lightEntity : lightView)
        {
			auto [transformComponent, directionalLight] = lightView.get<TransformComponent, DirectionalLightComponent>(lightEntity);

            directionalLightDir = directionalLight.WorldDirection;
            
            break; // Asumimos un solo Sol
        }
        
        CalculateCascadeMatrices(directionalLightDir);
    }

   void ShadowSystem::CalculateCascadeMatrices(const Math::Vector3& lightDir)
    {
        m_cascadesData.resize(m_numCascades);

        // 1. Garantizar dirección unitaria (evita colapsar la distancia de baseLookAt)
        Math::Vector3 lightDirNormalized = lightDir;
        lightDirNormalized.Normalize();

        float nearClip = m_camera.GetNearPlane();
        float farClip  = m_camera.GetFarPlane();
        // Si tienes m_maxShadowDistance configurado a un valor finito (ej. 150.0f - 500.0f):
        // farClip = (std::min)(farClip, m_maxShadowDistance);

        float fov    = m_camera.GetFieldOfView();
        float aspect = m_camera.GetViewport().AspectRatio();

        // 2. Practical Split Scheme
        std::vector<float> splitDistances(m_numCascades + 1);
        for (uint32_t i = 0; i <= m_numCascades; ++i)
        {
            float p = static_cast<float>(i) / static_cast<float>(m_numCascades);
            float logC = nearClip * std::pow(farClip / nearClip, p);
            float uniC = nearClip + (farClip - nearClip) * p;
            splitDistances[i] = logC * m_cascadeLambda + uniC * (1.0f - m_cascadeLambda);
        }

        Math::Matrix camViewInv = m_camera.GetViewInverse();

        for (uint32_t i = 0; i < m_numCascades; ++i)
        {
            float cascadeNear = splitDistances[i];
            float cascadeFar  = splitDistances[i + 1];

            // Solapamos el plano cercano de las cascadas 1..N un 15% hacia atrás para cubrir el blendBand
            float frustumNear = cascadeNear;
            if (i > 0)
            {
                float prevDepth = splitDistances[i] - splitDistances[i - 1];
                frustumNear = (std::max)(nearClip, cascadeNear - prevDepth * 0.15f);
            }
            
            // 3. Esquinas del sub-frustum en View Space (RH: cámara mira hacia -Z)
            float tanHalfFov = std::tan(fov * 0.5f);
            float nearY = frustumNear * tanHalfFov;
            float nearX = nearY * aspect;
            float farY  = cascadeFar * tanHalfFov;
            float farX  = farY * aspect;

            //Calcular frustums usando sistema RH
            Math::Vector3 frustumCorners[8] = {
                Math::Vector3(-nearX,  nearY, -frustumNear), Math::Vector3( nearX,  nearY, -frustumNear),
                Math::Vector3( nearX, -nearY, -frustumNear), Math::Vector3(-nearX, -nearY, -frustumNear),
                Math::Vector3(-farX,   farY,  -cascadeFar),  Math::Vector3( farX,   farY,  -cascadeFar),
                Math::Vector3( farX,  -farY,  -cascadeFar),  Math::Vector3(-farX,  -farY,  -cascadeFar)
            };

            Math::Vector3 center = Math::Vector3::Zero;
            for (int j = 0; j < 8; ++j)
            {
                frustumCorners[j] = Math::Vector3::Transform(frustumCorners[j], camViewInv);
                center += frustumCorners[j];
            }
            center /= 8.0f;

            float sphereRadius = 0.0f;
            for (int j = 0; j < 8; ++j)
            {
                float dist = Math::Vector3::Distance(center, frustumCorners[j]);
                sphereRadius = std::max<float>(sphereRadius, dist);
            }

            // Estabilizar el radio para evitar oscilaciones al rotar
            sphereRadius = std::ceil(sphereRadius * 16.0f) / 16.0f;

            // 4. Texel Snapping exclusivamente en X e Y (sin alterar la profundidad Z)
            float texelPerUnit = static_cast<float>(m_shadowMapResolution) / (sphereRadius * 2.0f);
            Math::Matrix scalar = Math::Matrix::CreateScale(texelPerUnit, texelPerUnit, 1.0f);

            Math::Vector3 upDir = (std::abs(lightDirNormalized.y) > 0.99f) ? Math::Vector3::Forward : Math::Vector3::Up;
            Math::Vector3 baseLookAt = center - (lightDirNormalized * sphereRadius);
            Math::Matrix lightView = Math::Matrix::CreateLookAt(baseLookAt, center, upDir);

            lightView = lightView * scalar;
            lightView.m[3][0] = std::floor(lightView.m[3][0]);
            lightView.m[3][1] = std::floor(lightView.m[3][1]);
            lightView = lightView * scalar.Invert();

            // 5. Holgura de 2 texeles para absorber el floor() + rango Z amplio para oclusores altos
            float worldUnitsPerTexel = 1.0f / texelPerUnit;
            float paddedRadius = sphereRadius + (worldUnitsPerTexel * 2.0f);
            float zMargin = std::max<float>(sphereRadius * 6.0f, 1000.0f);

            Math::Matrix lightProj = Math::Matrix::CreateOrthographicOffCenter(
                -paddedRadius, paddedRadius,
                -paddedRadius, paddedRadius,
                -zMargin,
                zMargin
            );

            auto& cascade = m_cascadesData[i];
            cascade.LightViewProj = lightView * lightProj;
            cascade.SplitDistance = cascadeFar;
        }
    }
}
