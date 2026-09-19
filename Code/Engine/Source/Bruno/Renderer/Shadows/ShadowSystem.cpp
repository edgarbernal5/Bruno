#include "brpch.h"
#include "ShadowSystem.h"

#include "CascadedShadows.h"
#include "Bruno/Renderer/Camera.h"

namespace Bruno
{
    ShadowSystem::ShadowSystem(Camera& camera) : 
        m_camera(camera)
    {
    }

    void ShadowSystem::Execute()
    {
        // 1. Extraemos las matrices de tu método robusto
        auto cascades = CalculateCascadeMatrices(m_camera, m_directionalLightDir, m_numCascades, m_shadowMapResolution, m_cascadeLambda);

    }

    std::vector<CascadeData> ShadowSystem::CalculateCascadeMatrices(const Camera& camera, 
        const Math::Vector3& lightDir, 
        uint32_t numCascades, 
        float shadowMapResolution,
        float cascadeLambda)
    {
        std::vector<CascadeData> cascades(numCascades);
        
        float nearClip = camera.GetNearPlane();
        float farClip = camera.GetFarPlane();
        float fov = camera.GetFieldOfView();
        float aspect = camera.GetViewport().AspectRatio();

        // 1. Calcular las distancias Z de cada corte (Practical Split Scheme)
        std::vector<float> splitDistances(numCascades + 1);
        for (uint32_t i = 0; i <= numCascades; ++i)
        {
            float p = static_cast<float>(i) / static_cast<float>(numCascades);
            float logC = nearClip * std::pow(farClip / nearClip, p);
            float uniC = nearClip + (farClip - nearClip) * p;
            splitDistances[i] = logC * cascadeLambda + uniC * (1.0f - cascadeLambda);
        }

        Math::Matrix camViewInv = camera.GetViewInverse();

        // 2. Iterar por cada cascada
        for (uint32_t i = 0; i < numCascades; ++i)
        {
            float cascadeNear = splitDistances[i];
            float cascadeFar  = splitDistances[i + 1];

            // 3. Extraer las 8 esquinas del sub-frustum en Espacio de Vista mediante trigonometría
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

            // 4. Centro geométrico en Espacio de Mundo
            Math::Vector3 center = Math::Vector3::Zero;
            for (int j = 0; j < 8; ++j)
            {
                frustumCorners[j] = Math::Vector3::Transform(frustumCorners[j], camViewInv);
                center += frustumCorners[j];
            }
            center /= 8.0f;

            // 5. Matriz View de la Luz (mirando al centro del sub-frustum)
            Math::Vector3 lightPos = center - (lightDir * (farClip - nearClip)); 
            Math::Matrix lightView = Math::Matrix::CreateLookAt(lightPos, center, Math::Vector3::Up);

            // 6. Encontrar el Bounding Box en el Espacio de la Luz
            float minX = (std::numeric_limits<float>::max)();
            float maxX = (std::numeric_limits<float>::lowest());
            float minY =  (std::numeric_limits<float>::max)();
            float maxY = (std::numeric_limits<float>::lowest());
            float minZ =  (std::numeric_limits<float>::max)();
            float maxZ = (std::numeric_limits<float>::lowest());

            for (int j = 0; j < 8; ++j) {
                Math::Vector3 cornerLightSpace = Math::Vector3::Transform(frustumCorners[j], lightView);
                minX = std::min<float>(minX, cornerLightSpace.x);
                maxX = std::max<float>(maxX, cornerLightSpace.x);
                minY = std::min<float>(minY, cornerLightSpace.y);
                maxY = std::max<float>(maxY, cornerLightSpace.y);
                minZ = std::min<float>(minZ, cornerLightSpace.z);
                maxZ = std::max<float>(maxZ, cornerLightSpace.z);
            }

            // ==========================================
            // 7. LA MAGIA AAA 1: TEXEL SNAPPING
            // ==========================================
            float shadowOrthoSizeX = maxX - minX;
            float shadowOrthoSizeY = maxY - minY;
            
            float worldUnitsPerTexelX = shadowOrthoSizeX / shadowMapResolution;
            float worldUnitsPerTexelY = shadowOrthoSizeY / shadowMapResolution;

            // Anclamos los bordes a múltiplos exactos del texel para anular el parpadeo
            minX = std::floor(minX / worldUnitsPerTexelX) * worldUnitsPerTexelX;
            maxX = std::floor(maxX / worldUnitsPerTexelX) * worldUnitsPerTexelX;
            minY = std::floor(minY / worldUnitsPerTexelY) * worldUnitsPerTexelY;
            maxY = std::floor(maxY / worldUnitsPerTexelY) * worldUnitsPerTexelY;

            // ==========================================
            // 8. LA MAGIA AAA 2: Z-PULLBACK (Shadow Popping Fix)
            // ==========================================
            // Tiramos el plano cercano artificialmente hacia atrás hacia la luz
            float lightNearZ = minZ - 150.0f; 
            float lightFarZ  = maxZ;

            // 9. Construir proyección final[cite: 4]
            Math::Matrix lightProj = Math::Matrix::CreateOrthographicOffCenter(
                minX, maxX, minY, maxY, lightNearZ, lightFarZ
            );

            cascades[i].LightViewProj = lightView * lightProj;
            cascades[i].SplitDistance = cascadeFar; 
        }

        return cascades;
    }
}
