#include "brpch.h"
#include "CullingSystem.h"

#include "Bruno/Core/JobSystem.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Components.h"
#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    CullingSystem::CullingSystem(Camera& camera, std::shared_ptr<Scene> scene, std::shared_ptr<ShadowSystem> shadowSystem) : 
        m_camera(camera),
        m_scene(scene),
        m_shadowSystem(shadowSystem)
    {
    }

    void CullingSystem::Execute()
    {
        m_finalResults.MainCamera.clear();
        for (int i = 0; i < NUM_CASCADES; ++i)
        {
            m_finalResults.Cascades[i].clear();
        }
        
        // Calcular ViewProjection y extraer los 6 planos universales
        DirectX::XMVECTOR frustumPlanes[6];
        ExtractFrustumPlanes(frustumPlanes);
        
        DirectX::BoundingOrientedBox cascadeOBBs[NUM_CASCADES];
        auto lightView = m_scene->GetAllEntitiesWith<DirectionalLightComponent>();
        
        for (auto entity : lightView)
        {
            const auto& light = lightView.get<DirectionalLightComponent>(entity);
            for (uint32_t i = 0; i < NUM_CASCADES; ++i)
            {
                cascadeOBBs[i] = CreateOBBFromMatrix(light.LightViewProj[i]);
            }
            break; // Asumimos un solo Sol
        }
        
        // Obtener todas las entidades con los componentes necesarios
        auto entitiesGroup = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent, BoundingBoxComponent>();
        std::vector<entt::entity> entitiesToCull(entitiesGroup.begin(), entitiesGroup.end());
        uint32_t entityCount = static_cast<uint32_t>(entitiesToCull.size());
        if (entityCount == 0)
        {
            return;
        }
        
        // Definir el tamaño del chunk (ej. 1024 entidades por Hilo)
        const uint32_t chunkSize = 1024;
        const uint32_t numChunks = (entityCount + chunkSize - 1) / chunkSize;
        
        // FASE SETUP: Pre-alojar memoria para los hilos (Lock-free)
        PrepareMemoryChunks(numChunks, chunkSize);
        
        // Creamos nuestro "Grupo" para este pase de culling
        JobDispatchGroup cullingGroup;
        
        JobSystem::Get().Dispatch(entityCount, chunkSize, [&](uint32_t start, uint32_t end)
        {
            uint32_t chunkIndex = start / chunkSize;
            auto& localResult = m_cullingChunks[chunkIndex];
            
            for (uint32_t i = start; i < end; ++i)
            {
                entt::entity entity = entitiesToCull[i];
                const auto& bbox = entitiesGroup.get<BoundingBoxComponent>(entity);
                const auto& transform = entitiesGroup.get<TransformComponent>(entity);
                
                DirectX::BoundingOrientedBox localObb;
                localObb.Center = DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z);
                localObb.Extents = DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z);
                localObb.Orientation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

                Math::Matrix worldMat = transform.WorldTransform;
                const DirectX::XMFLOAT4X4* worldMatFloat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&worldMat);
                DirectX::XMMATRIX xmWorld = DirectX::XMLoadFloat4x4(worldMatFloat);
        
                DirectX::BoundingOrientedBox worldObb;
                localObb.Transform(worldObb, xmWorld);

                // A. ¿Cámara Principal?
                // Usamos la función nativa ContainedBy contra nuestros planos perfectos
                // Orden: Near, Far, Right, Left, Top, Bottom
                DirectX::ContainmentType result = worldObb.ContainedBy(
                    frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], 
                    frustumPlanes[3], frustumPlanes[4], frustumPlanes[5]
                );

                if (result != DirectX::DISJOINT)
                {
                    localResult.VisibleEntities.emplace_back(entity);
                }

                // B. ¿Cascadas de Sombras? (SIMD Ultra Rápido sin locks)
                for (uint32_t c = 0; c < NUM_CASCADES; ++c)
                {
                    //if (cascadeOBBs[c].Intersects(worldOBB))
                    {
                        localResult.ShadowCascades[c].emplace_back(entity);
                    }
                }
            }
        }, &cullingGroup);
        
        // Vector de vectores para guardar resultados SIN mutexes
        /*
        std::vector<ThreadLocalResult> threadLocalVisible(numChunks);
        for (size_t chunkIdx = 0; chunkIdx < numChunks; ++chunkIdx)
        {
            JobSystem::Get().Execute([&, chunkIdx]()
            {
                size_t startIdx = chunkIdx * chunkSize;
                size_t endIdx = std::min<size_t>(startIdx + chunkSize, totalEntities);
                
                // Reservamos memoria aproximada para evitar allocations
                threadLocalVisible[chunkIdx].visibleEntities.reserve(chunkSize / 2);
                
                for (size_t i = startIdx; i < endIdx; ++i)
                {
                    entt::entity entt = entitiesToCull[i];
                    const auto& [transform, modelComponent, bbox] = entitiesGroup.get<TransformComponent, ModelComponent, BoundingBoxComponent>(entt);
                    
                    DirectX::BoundingOrientedBox localObb;
                    localObb.Center = DirectX::XMFLOAT3(bbox.Center.x, bbox.Center.y, bbox.Center.z);
                    localObb.Extents = DirectX::XMFLOAT3(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z);
                    localObb.Orientation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

                    Math::Matrix worldMat = transform.WorldTransform;
                    const DirectX::XMFLOAT4X4* worldMatFloat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&worldMat);
                    DirectX::XMMATRIX xmWorld = DirectX::XMLoadFloat4x4(worldMatFloat);
            
                    DirectX::BoundingOrientedBox worldObb;
                    localObb.Transform(worldObb, xmWorld);
                    
                    // Usamos la función nativa ContainedBy contra nuestros planos perfectos
                    // Orden: Near, Far, Right, Left, Top, Bottom
                    DirectX::ContainmentType result = worldObb.ContainedBy(
                        frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], 
                        frustumPlanes[3], frustumPlanes[4], frustumPlanes[5]
                    );

                    if (result != DirectX::DISJOINT)
                    {
                        threadLocalVisible[chunkIdx].visibleEntities.emplace_back(entt);
                    }
                }
            }, &cullingGroup);
        }
        */
        JobSystem::Get().Wait(cullingGroup);
        
        // FASE REDUCE: Unimos todos los WorkerChunks en m_finalResults
        ConsolidateResults(numChunks);
    }

    void CullingSystem::ExtractFrustumPlanes(DirectX::XMVECTOR planesOut[6])
    {
        // Asegurar compatibilidad de memoria con DirectXMath
        const DirectX::XMFLOAT4X4* vpF = &m_camera.GetViewProjection();

        // NOTA: DirectXMath's OBB::ContainedBy espera que las normales de los planos
        // apunten hacia AFUERA del frustum. (Distancia positiva = afuera).

        // 1. Near Plane (-z) (Asumiendo DirectX clip space 0 a W)
        planesOut[0] = DirectX::XMVectorSet(-vpF->_13, -vpF->_23, -vpF->_33, -vpF->_43);
    
        // 2. Far Plane (z - w)
        planesOut[1] = DirectX::XMVectorSet(vpF->_13 - vpF->_14, vpF->_23 - vpF->_24, vpF->_33 - vpF->_34, vpF->_43 - vpF->_44);
    
        // 3. Right Plane (x - w)
        planesOut[2] = DirectX::XMVectorSet(vpF->_11 - vpF->_14, vpF->_21 - vpF->_24, vpF->_31 - vpF->_34, vpF->_41 - vpF->_44);
    
        // 4. Left Plane (-w - x)
        planesOut[3] = DirectX::XMVectorSet(-vpF->_14 - vpF->_11, -vpF->_24 - vpF->_21, -vpF->_34 - vpF->_31, -vpF->_44 - vpF->_41);
    
        // 5. Top Plane (y - w)
        planesOut[4] = DirectX::XMVectorSet(vpF->_12 - vpF->_14, vpF->_22 - vpF->_24, vpF->_32 - vpF->_34, vpF->_42 - vpF->_44);
    
        // 6. Bottom Plane (-w - y)
        planesOut[5] = DirectX::XMVectorSet(-vpF->_14 - vpF->_12, -vpF->_24 - vpF->_22, -vpF->_34 - vpF->_32, -vpF->_44 - vpF->_42);

        // Normalizar los 6 planos (Obligatorio para que ContainedBy calcule las distancias reales)
        for (int i = 0; i < 6; ++i)
        {
            planesOut[i] = DirectX::XMPlaneNormalize(planesOut[i]);
        }
    }

    void CullingSystem::PrepareMemoryChunks(uint32_t numChunks, uint32_t chunkSize)
    {
        if (m_cullingChunks.size() < numChunks)
        {
            m_cullingChunks.resize(numChunks);
        }

        for (uint32_t i = 0; i < numChunks; ++i)
        {
            // Limpiamos resultados del frame anterior sin liberar la capacidad de RAM subyacente
            m_cullingChunks[i].VisibleEntities.clear();
            m_cullingChunks[i].VisibleEntities.reserve(chunkSize);
            
            for (uint32_t c = 0; c < NUM_CASCADES; ++c)
            {
                m_cullingChunks[i].ShadowCascades[c].clear();
                // Una reserva generosa, ya que cada cascada verá una fracción de la escena
                m_cullingChunks[i].ShadowCascades[c].reserve(chunkSize);
            }
        }
    }

    void CullingSystem::ConsolidateResults(uint32_t numChunks)
    {
        m_finalResults.MainCamera.clear();
        for (uint32_t c = 0; c < NUM_CASCADES; ++c)
        {
            m_finalResults.Cascades[c].clear();
        }

        // 1. Contar totales para hacer una sola alocación exacta de memoria maestra
        size_t totalVisible = 0;
        size_t totalShadows[NUM_CASCADES] = { 0 };

        for (uint32_t i = 0; i < numChunks; ++i)
        {
            totalVisible += m_cullingChunks[i].VisibleEntities.size();
            for (uint32_t c = 0; c < NUM_CASCADES; ++c)
            {
                totalShadows[c] += m_cullingChunks[i].ShadowCascades[c].size();
            }
        }

        m_finalResults.MainCamera.reserve(totalVisible);
        for (uint32_t c = 0; c < NUM_CASCADES; ++c)
        {
            m_finalResults.Cascades[c].reserve(totalShadows[c]);
        }

        // 2. Fusión masiva ultra rápida (Inserción O(n) contigua)
        for (uint32_t i = 0; i < numChunks; ++i)
        {
            m_finalResults.MainCamera.insert(
                m_finalResults.MainCamera.end(),
                m_cullingChunks[i].VisibleEntities.begin(),
                m_cullingChunks[i].VisibleEntities.end()
            );

            for (uint32_t c = 0; c < NUM_CASCADES; ++c)
            {
                m_finalResults.Cascades[c].insert(
                    m_finalResults.Cascades[c].end(),
                    m_cullingChunks[i].ShadowCascades[c].begin(),
                    m_cullingChunks[i].ShadowCascades[c].end()
                );
            }
        }
    }

    DirectX::BoundingOrientedBox CullingSystem::CreateOBBFromMatrix(const Math::Matrix& viewProj)
    {
        // Esquinas exactas del espacio NDC de DirectX 12
        Math::Vector3 ndcCorners[8] = {
            Math::Vector3(-1.0f,  1.0f, 0.0f), Math::Vector3( 1.0f,  1.0f, 0.0f),
            Math::Vector3( 1.0f, -1.0f, 0.0f), Math::Vector3(-1.0f, -1.0f, 0.0f),
            Math::Vector3(-1.0f,  1.0f, 1.0f), Math::Vector3( 1.0f,  1.0f, 1.0f),
            Math::Vector3( 1.0f, -1.0f, 1.0f), Math::Vector3(-1.0f, -1.0f, 1.0f)
        };

        Math::Matrix invViewProj = viewProj.Invert();
        Math::Vector3 worldCorners[8];

        // Llevamos las esquinas del cubo hacia el espacio del mundo real
        for (int i = 0; i < 8; ++i)
        {
            worldCorners[i] = Math::Vector3::Transform(ndcCorners[i], invViewProj);
        }

        // Dejamos que DirectXMath calcule el Centro, la Rotación y las Extensiones de esa caja
        DirectX::BoundingOrientedBox cascadeOBB;
        DirectX::BoundingOrientedBox::CreateFromPoints(cascadeOBB, 8, (const DirectX::XMFLOAT3*)worldCorners, sizeof(Math::Vector3));
    
        return cascadeOBB;
    }
}
