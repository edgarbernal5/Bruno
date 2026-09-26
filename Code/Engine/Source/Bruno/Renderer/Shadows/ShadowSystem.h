#pragma once
#include "Bruno/Math/Math.h"
#include "Bruno/Scene/Constants.h"

namespace Bruno
{
    class Scene;
    class Camera;
    
    struct CascadeData
    {
        Math::Matrix LightViewProj;
        float SplitDistance; // Distancia Z en espacio de vista donde termina la cascada
    };

    class ShadowSystem
    {
    public:
        ShadowSystem(Camera& camera, std::shared_ptr<Scene> scene);
        
        void Execute();
        
        const std::vector<CascadeData>& GetCascades() const { return m_cascadesData; }
        void SetCascadeLambda(float lambda) { m_cascadeLambda = lambda; }
    private:
        void CalculateCascadeMatrices(const Math::Vector3& lightDir);
        
        Camera& m_camera;
        std::shared_ptr<Scene> m_scene;
        std::vector<CascadeData> m_cascadesData;
        uint32_t m_numCascades = NUM_CASCADES;
        float m_cascadeLambda = 0.5f; // 0.0 = Uniforme, 1.0 = Logarítmico
        uint32_t m_shadowMapResolution = SHADOW_MAP_RES; // Resolución de tu textura de sombras
    };
}
