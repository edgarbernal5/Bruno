#pragma once

#include "Bruno/Math/Math.h"

#include "Bruno/Scene/Constants.h"

namespace Bruno
{
    // Estructura para alinear con el cbuffer ShadowConstants
    struct ShadowConstants
    {
        Math::Matrix World;
        Math::Matrix LightViewProj;
    };
    
    enum class ShadowFilterQuality
    {
        PCF3x3,
        PCF5x5,
        
    };
    
    struct ShadowProperties
    {
        uint32_t m_numCascades = NUM_CASCADES;
        float m_maxShadowDistance = FLT_MAX;
        uint32_t m_resolution = 2048;
        float m_cascadeLambda = 0.5f;
        int m_depthBias = 50000;
        float m_slopeScaledDepthBias = 2048;
        ShadowFilterQuality m_filterQuality = ShadowFilterQuality::PCF3x3;
    };
}
