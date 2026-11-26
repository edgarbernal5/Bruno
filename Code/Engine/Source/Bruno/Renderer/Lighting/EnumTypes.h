#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
    struct alignas(16) DirectionalLightData
    {
        Math::Vector3 Direction;
        float Intensity;
        Math::Vector3 Color;
        float Padding; // Relleno obligatorio
    };
	
    struct PointLightData
    {
        Math::Vector3 Position;
        float Radius;
        Math::Vector3 Color;
        float Intensity;
    };
	
    struct SpotLightData
    {
        Math::Vector3 Position;
        float Radius; // Distancia máxima de influencia (como la Point Light)
        Math::Vector3 Direction;
        float Intensity;
        Math::Vector3 Color;
        float InnerConeCos; // std::cos(InnerCutoffAngle)
        float OuterConeCos; // std::cos(OuterCutoffAngle)
        Math::Vector3 Padding;
    };
}
