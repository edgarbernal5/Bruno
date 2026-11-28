#ifndef PBR_COMMON_HLSLI
#define PBR_COMMON_HLSLI

#include "Common.hlsli"

// ==============================================================================
// FUNCIONES DE DISTRIBUCIÓN, GEOMETRÍA Y FRESNEL (Cook-Torrance)
// ==============================================================================
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    return NdotV / (NdotV * (1.0f - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);

    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

// ==============================================================================
// EVALUACIÓN RADIOMÉTRICA MAESTRA (Direccional, Puntual o Spot)
// ==============================================================================
float3 CalculatePBRIllumination(
    float3 F0,
    float3 albedo,
    float metallic,
    float roughness,
    float3 N,
    float3 V,
    float3 L,
    float3 radiance)
{
    float NdotL = max(dot(N, L), 0.0f);
    if (NdotL <= 0.0f)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    float3 H    = normalize(V + L);
    float NdotV = max(dot(N, V), 0.0f);

    float  NDF = DistributionGGX(N, H, roughness);
    float  G   = GeometrySmith(N, V, L, roughness);
    float3 F   = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float3 numerator   = NDF * G * F;
    float  denominator = 4.0f * NdotV * NdotL;
    float3 specular    = numerator / max(denominator, 0.0000001f);

    // Conservación de energía: los metales puros absorben la componente difusa[cite: 6]
    float3 kS = F;
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1.0f - metallic);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// ==============================================================================
// ATENUACIÓN VOLUMÉTRICA PARA LUCES LOCALES (Inversa del cuadrado ventaneada)
// ==============================================================================
float CalculateDistanceAttenuation(float distance, float radius)
{
    float distRatio = saturate(distance / max(radius, 0.0001f));
    float d2 = distRatio * distRatio;
    float window = saturate(1.0f - d2 * d2);

    return (window * window) / (distance * distance + 1.0f);
}

float CalculateSpotAttenuation(float3 L, float3 spotDirection, float innerConeCos, float outerConeCos)
{
    float theta   = dot(-L, normalize(spotDirection));
    float epsilon = max(innerConeCos - outerConeCos, 0.0001f);
    float spot    = saturate((theta - outerConeCos) / epsilon);

    return spot * spot; // Suavizado smoothstep
}

#endif // PBR_COMMON_HLSLI