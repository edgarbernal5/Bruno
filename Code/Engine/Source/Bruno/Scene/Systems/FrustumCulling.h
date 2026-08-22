#pragma once

#include <DirectXMath.h>
#include <vector>

#include "Bruno/Scene/Entity.h"

namespace Bruno
{
    class Camera;
    class Entity;
    class Scene;
    
    struct alignas(std::hardware_destructive_interference_size) ThreadLocalResult
    {
        std::vector<entt::entity> visibleEntities;
    };
    
    class FrustumCulling
    {
    public:
        FrustumCulling(Camera& camera, std::shared_ptr<Scene> scene);
        
        void Update();
        
        const std::vector<Entity>& GetVisibleEntities() { return m_visibleEntities; }
    private:
        void ExtractFrustumPlanes(DirectX::XMVECTOR planesOut[6]);
        
        Camera& m_camera;
        std::shared_ptr<Scene> m_scene;
        std::vector<Entity> m_visibleEntities;    
    };
}
