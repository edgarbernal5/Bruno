#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
    // Estructura para alinear con el cbuffer ShadowConstants
    struct ShadowConstants
    {
        Math::Matrix World;
        Math::Matrix LightViewProj;
    };
}
