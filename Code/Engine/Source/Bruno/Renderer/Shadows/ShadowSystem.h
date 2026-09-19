#pragma once
#include "Bruno/Math/Math.h"
#include "Bruno/Scene/Constants.h"

namespace Bruno
{
    class Camera;
    
    struct CascadeData
    {
        Math::Matrix LightViewProj;
        float SplitDistance; // Distancia Z en espacio de vista donde termina la cascada
    };

    class ShadowSystem
    {
    public:
        ShadowSystem(Camera& camera);
        
        void Execute();
        void SetCascadeLambda(float lambda) { m_cascadeLambda = lambda; }
    private:
        std::vector<CascadeData> CalculateCascadeMatrices(
            const Camera& camera, 
            const Math::Vector3& lightDir, 
            uint32_t numCascades, 
            float shadowMapResolution, // Ej. 2048.0f
            float cascadeLambda = 0.5f);
        
        Camera& m_camera;
        
        uint32_t m_numCascades = NUM_CASCADES;
        float m_cascadeLambda = 0.5f; // 0.0 = Uniforme, 1.0 = Logarítmico
        uint32_t m_shadowMapResolution = 2048; // Resolución de tu textura de sombras
    };
}
