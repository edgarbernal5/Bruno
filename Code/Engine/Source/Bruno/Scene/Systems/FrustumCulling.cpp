#include "brpch.h"
#include "FrustumCulling.h"

#include "Bruno/Platform/DirectX/VertexTypes.h"
#include "Bruno/Renderer/Camera.h"
#include "Bruno/Scene/Components.h"
#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    FrustumCulling::FrustumCulling(Camera& camera, std::shared_ptr<Scene> scene) : 
        m_camera(camera),
        m_scene(scene)
    {
    }

    void FrustumCulling::Update()
    {
        // Calcular ViewProjection y extraer los 6 planos universales
        DirectX::XMVECTOR frustumPlanes[6];
        ExtractFrustumPlanes(frustumPlanes);
		
        m_visibleEntities.clear();
        
        auto entitiesCull = m_scene->GetAllEntitiesWith<TransformComponent, ModelComponent, BoundingBoxComponent>();
        for (auto& entt : entitiesCull)
        {
            const auto& [transform, modelComponent, bbox] = entitiesCull.get<TransformComponent, ModelComponent, BoundingBoxComponent>(entt);
			
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
                m_visibleEntities.emplace_back(entt, m_scene.get());
            }
        }
    }

    void FrustumCulling::ExtractFrustumPlanes(DirectX::XMVECTOR planesOut[6])
    {
        // Asegurar compatibilidad de memoria con DirectXMath
        const DirectX::XMFLOAT4X4* vpF = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&m_camera.GetViewProjection());

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
}
