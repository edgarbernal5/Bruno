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
        None,
        PCF3x3,
        PCF5x5,
        PCSS
    };
    
    struct ShadowProperties
    {
        uint32_t m_numCascades = MAX_CASCADES;
        float m_maxShadowDistance = FLT_MAX;
        uint32_t m_resolution = SHADOW_MAP_RES;
        float m_ShadowLightSize = 0.05f;
        float m_cascadeLambda = 0.5f;
        int m_depthBias = 50000;
        float m_slopeScaledDepthBias = 1.0f;
        ShadowFilterQuality m_filterQuality = ShadowFilterQuality::None;
    };
}
