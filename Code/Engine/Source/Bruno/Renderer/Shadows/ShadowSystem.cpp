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
        
        // 1. Extraemos las matrices de tu método robusto
        CalculateCascadeMatrices(directionalLightDir);
    }

   void ShadowSystem::CalculateCascadeMatrices(const Math::Vector3& lightDir)
    {
        m_cascadesData.resize(m_numCascades);
        
        float nearClip = m_camera.GetNearPlane();
        float farClip = m_camera.GetFarPlane();
        float fov = m_camera.GetFieldOfView();
        float aspect = m_camera.GetViewport().AspectRatio();

        // 1. Calcular las distancias Z de cada corte (Practical Split Scheme)
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

            // Extraer las 8 esquinas del sub-frustum en Espacio de Vista mediante trigonometría
            float tanHalfFov = std::tan(fov * 0.5f);
            float nearY = cascadeNear * tanHalfFov;
            float nearX = nearY * aspect;
            float farY  = cascadeFar * tanHalfFov;
            float farX  = farY * aspect;

            Math::Vector3 frustumCorners[8] = {
                Math::Vector3(-nearX,  nearY, cascadeNear), Math::Vector3( nearX,  nearY, cascadeNear),
                Math::Vector3( nearX, -nearY, cascadeNear), Math::Vector3(-nearX, -nearY, cascadeNear),
                Math::Vector3(-farX,  farY, cascadeFar),  Math::Vector3( farX,  farY, cascadeFar),
                Math::Vector3( farX, -farY, cascadeFar),  Math::Vector3(-farX, -farY, cascadeFar)
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

            float texelPerUnit = static_cast<float>(m_shadowMapResolution) / (sphereRadius * 2.0f);
            Math::Matrix scalar = Math::Matrix::CreateScale(texelPerUnit, texelPerUnit, texelPerUnit);
            Math::Vector3 baseLookAt = center - (lightDir * sphereRadius);
            Math::Matrix lightView = Math::Matrix::CreateLookAt(baseLookAt, center, Math::Vector3::Up);
            
            lightView = lightView * scalar;
            lightView.m[3][0] = std::floor(lightView.m[3][0]);
            lightView.m[3][1] = std::floor(lightView.m[3][1]);
            lightView.m[3][2] = std::floor(lightView.m[3][2]);
            lightView = lightView * scalar.Invert();
            
            Math::Matrix lightProj = Math::Matrix::CreateOrthographicOffCenter(
            -sphereRadius, sphereRadius, 
            -sphereRadius, sphereRadius, 
            -sphereRadius * 6.0f, // Con DepthClipEnable=FALSE, este valor puede ser compacto
            sphereRadius * 6.0f
        );
            
            auto& cascade = m_cascadesData[i];
            
            cascade.LightViewProj = lightView * lightProj;
            cascade.SplitDistance = cascadeFar; 
        }
    }
}
