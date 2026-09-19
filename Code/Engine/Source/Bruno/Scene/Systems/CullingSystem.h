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
    
    
    struct alignas(64) CullingChunkResult
    {
        std::vector<entt::entity> VisibleEntities;
        std::vector<entt::entity> ShadowCascades[NUM_CASCADES];
    };
    
    struct FrameCullingResults
    {
        std::vector<entt::entity> MainCamera;
        std::vector<entt::entity> Cascades[NUM_CASCADES];
    };
    
    class CullingSystem
    {
    public:
        CullingSystem(Camera& camera, std::shared_ptr<Scene> scene);
        
        void Execute();
        
        const std::vector<Entity>& GetVisibleEntities() { return m_visibleEntities; }
        size_t GetTotalEntities() const { return m_totalEntities; }
        size_t GetTotalVisibleEntities() const { return m_totalVisibleCount; }
        const FrameCullingResults& GetCullingResults() const { return m_finalResults; }
    private:
        void ExtractFrustumPlanes(DirectX::XMVECTOR planesOut[6]);
        
        // Métodos internos de Map-Reduce (Cero alocaciones dinámicas en el bucle)
        void PrepareMemoryChunks(uint32_t numChunks, uint32_t chunkSize);
        void ConsolidateResults(uint32_t numChunks);
        
        DirectX::BoundingOrientedBox CreateOBBFromMatrix(const Math::Matrix& viewProj);
        
        Camera& m_camera;
        size_t m_totalEntities { 0 };
        size_t m_totalVisibleCount { 0 };
        std::shared_ptr<Scene> m_scene;
        std::vector<Entity> m_visibleEntities;
        
        std::vector<CullingChunkResult> m_cullingChunks;
        
        std::vector<CullingChunkResult> m_workerChunks;
        FrameCullingResults m_finalResults;
        
        std::vector<entt::entity> m_finalVisibleEntities;
        std::vector<entt::entity> m_finalShadowEntities[NUM_CASCADES];
    };
}
