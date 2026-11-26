#ifndef SHADOWS_COMMON_HLSLI
#define SHADOWS_COMMON_HLSLI

#ifndef MAX_CASCADES
#define MAX_CASCADES 4
#endif

#include "Common.hlsli"

// ==============================================================================
// CONTRATO DE PARÁMETROS CSM (Agnóstico a la Root Signature)
// ==============================================================================
struct CSMParams
{
    float4x4 LightSpaceMatrices[MAX_CASCADES];
    float4   CascadeSplits;
    uint     NumCascades;
    uint     FilterQuality;         // 0 = Hard, 1 = PCF 3x3, 2 = PCF 5x5, 3 = PCSS
    float    ShadowMapResolution;
    float    ShadowLightSize;
    float4   CascadeTexelBias;
};

// Interleaved Gradient Noise (IGN)
float InterleavedGradientNoise(float2 pixelPos)
{
    float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
    return frac(magic.z * frac(dot(pixelPos, magic.xy)));
}

// Generación de coordenadas polares (Discos de Vogel)
float2 VogelDiskSample(int sampleIndex, int samplesCount, float noiseAngle)
{
    float r = sqrt((float)sampleIndex + 0.5f) / sqrt((float)samplesCount);
    float theta = sampleIndex * 2.4f + noiseAngle;
    return float2(cos(theta), sin(theta)) * r;
}

float ComputeCascadeBias(uint cascadeIndex, float3 normal, float3 lightDir, CSMParams params)
{
    float unitBias = max(params.CascadeTexelBias[cascadeIndex], 0.0001f);

    float filterRadiusScale = 1.0f;
    if (params.FilterQuality == 1)      filterRadiusScale = 1.5f; // PCF 3x3
    else if (params.FilterQuality >= 2) filterRadiusScale = 2.5f; // PCF 5x5 y PCSS

    float cosTheta = clamp(dot(normal, lightDir), 0.05f, 1.0f);
    float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float slope = sinTheta / cosTheta;

    float baseBias  = unitBias * filterRadiusScale;
    float slopeBias = baseBias * min(slope, 3.0f);

    return clamp(baseBias + slopeBias, unitBias * 0.5f, unitBias * 5.0f);
}

// --- Hard Shadows ---
float EvaluateHardShadow(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    float3 projCoords,
    uint cascadeIndex,
    float bias)
{
    return shadowMap.SampleCmpLevelZero(
        shadowSampler,
        float3(projCoords.xy, cascadeIndex),
        projCoords.z - bias
    );
}

// --- PCF 3x3 (Acelerado por Hardware) ---
float EvaluatePCF3x3(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    float3 projCoords,
    uint cascadeIndex,
    float bias,
    float texelSize)
{
    float shadow = 0.0f;
    float2 offsets[4] = {
        float2(-0.5f, -0.5f),
        float2( 0.5f, -0.5f),
        float2(-0.5f,  0.5f),
        float2( 0.5f,  0.5f)
    };

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        shadow += shadowMap.SampleCmpLevelZero(
            shadowSampler,
            float3(projCoords.xy + offsets[i] * texelSize, cascadeIndex),
            projCoords.z - bias
        );
    }
    return shadow * 0.25f;
}

// --- PCF 5x5 (Acelerado por Hardware) ---
float EvaluatePCF5x5(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    float3 projCoords,
    uint cascadeIndex,
    float bias,
    float texelSize)
{
    float shadow = 0.0f;

    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * 1.5f;
            shadow += shadowMap.SampleCmpLevelZero(
                shadowSampler,
                float3(projCoords.xy + offset * texelSize, cascadeIndex),
                projCoords.z - bias
            );
        }
    }
    return shadow / 9.0f;
}

// --- Percentage-Closer Soft Shadows (PCSS) ---
float EvaluatePCSS(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    SamplerState linearSampler,
    float3 projCoords,
    uint cascadeIndex,
    float bias,
    float2 screenPos,
    CSMParams params)
{
    float noiseAngle = InterleavedGradientNoise(screenPos) * 2.0f * PI;

    // 1. Blocker Search
    int blockerCount = 0;
    float blockerSum = 0.0f;

    //float searchRadius = params.ShadowLightSize * projCoords.z;
	float searchRadius = clamp(params.ShadowLightSize * projCoords.z, 0.0f, 0.04f);
    const int searchSamples = 16;

    for (int i = 0; i < searchSamples; ++i)
    {
        float2 offset = VogelDiskSample(i, searchSamples, noiseAngle) * searchRadius;
        float depth = shadowMap.SampleLevel(linearSampler, float3(projCoords.xy + offset, cascadeIndex), 0).r;

        if (depth < projCoords.z - bias)
        {
            blockerSum += depth;
            blockerCount++;
        }
    }

    if (blockerCount == 0) return 1.0f;
    if (blockerCount == searchSamples) return 0.0f;

    float avgBlockerDepth = blockerSum / (float)blockerCount;

    // 2. Estimación de Penumbra
    //float penumbraRatio = max(projCoords.z - avgBlockerDepth, 0.0f) / max(avgBlockerDepth, 0.0001f);
    //float filterRadius = penumbraRatio * params.ShadowLightSize;
	float penumbraRatio = max(projCoords.z - avgBlockerDepth, 0.0f) / max(avgBlockerDepth, 0.001f);
	float filterRadius  = clamp(penumbraRatio * params.ShadowLightSize, 0.0f, 0.04f);

    // 3. Filtrado PCF estocástico
    float shadow = 0.0f;
    const int pcfSamples = 16;

    for (int j = 0; j < pcfSamples; ++j)
    {
        float2 offset = VogelDiskSample(j, pcfSamples, noiseAngle) * filterRadius;
        shadow += shadowMap.SampleCmpLevelZero(
            shadowSampler,
            float3(projCoords.xy + offset, cascadeIndex),
            projCoords.z - bias
        );
    }

    return shadow / (float)pcfSamples;
}

// Evalúa una cascada individual y devuelve false si el punto cae fuera de su volumen
bool SampleCascadeFiltered(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    SamplerState linearSampler,
    float3 worldPos,
    uint cascadeIndex,
    float bias,
    float texelSize,
    float2 screenPos,
    bool isSecondaryTap,
    CSMParams params,
    out float outShadow)
{
    float4 lightSpacePos = mul(float4(worldPos, 1.0f), params.LightSpaceMatrices[cascadeIndex]);
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    projCoords.x =  projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f ||
        projCoords.z < 0.0f || projCoords.z > 1.0f)
    {
        outShadow = 1.0f;
        return false;
    }

    uint quality = params.FilterQuality;
    if (isSecondaryTap && quality > 1)
    {
        quality = 1;
    }

    if (quality == 0)
    {
        outShadow = EvaluateHardShadow(shadowMap, shadowSampler, projCoords, cascadeIndex, bias);
    }
    else if (quality == 1)
    {
        outShadow = EvaluatePCF3x3(shadowMap, shadowSampler, projCoords, cascadeIndex, bias, texelSize);
    }
    else if (quality == 2)
    {
        outShadow = EvaluatePCF5x5(shadowMap, shadowSampler, projCoords, cascadeIndex, bias, texelSize);
    }
    else
    {
        outShadow = EvaluatePCSS(shadowMap, shadowSampler, linearSampler, projCoords, cascadeIndex, bias, screenPos, params);
    }

    return true;
}

// ==========================================================
// CASCADED SHADOW MAPS (CSM)
// ==========================================================
float CalculateDirectionalShadow(
    Texture2DArray<float> shadowMap,
    SamplerComparisonState shadowSampler,
    SamplerState linearSampler,
    float3 worldPos,
    float viewDistance,
    float3 normal,
    float3 lightDir,
    float2 screenPos,
    CSMParams params,
    out uint outCascadeIndex)
{
    uint numCascades = min(params.NumCascades, (uint)MAX_CASCADES);
	if (numCascades == 0)
	{
    	outCascadeIndex = 0;
    	return 1.0f;
	}

    uint cascadeIndex = 0;
    uint limit = numCascades - 1;

    [unroll]
	for (uint i = 0; i < MAX_CASCADES - 1; ++i)
	{
    	if (i < limit && viewDistance > params.CascadeSplits[i])
    	{
			cascadeIndex = i + 1;
    	}
	}

    outCascadeIndex = cascadeIndex;

    float currentBias = ComputeCascadeBias(cascadeIndex, normal, lightDir, params);
    float texelSize   = 1.0f / max(params.ShadowMapResolution, 1.0f);

    // 1. Evaluación de la cascada principal
    float shadow = 1.0f;
    if (!SampleCascadeFiltered(shadowMap, shadowSampler, linearSampler, worldPos, cascadeIndex, currentBias, texelSize, screenPos, false, params, shadow))
    {
        return 1.0f;
    }

    // 2. Cascade Blending en el último 15% de profundidad de la cascada actual
    if (cascadeIndex < limit)
    {
        float splitFar     = params.CascadeSplits[cascadeIndex];
        float splitNear    = (cascadeIndex == 0) ? 0.0f : params.CascadeSplits[cascadeIndex - 1];
        float cascadeDepth = max(splitFar - splitNear, 0.001f);

        float blendBand   = cascadeDepth * 0.15f;
        float blendFactor = smoothstep(splitFar - blendBand, splitFar, viewDistance);

        [branch]
        if (blendFactor > 0.0f)
        {
            float nextBias   = ComputeCascadeBias(cascadeIndex + 1, normal, lightDir, params);
            float nextShadow = 1.0f;
            if (SampleCascadeFiltered(shadowMap, shadowSampler, linearSampler, worldPos, cascadeIndex + 1, nextBias, texelSize, screenPos, true, params, nextShadow))
            {
                shadow = lerp(shadow, nextShadow, blendFactor);
            }
        }
    }

    return shadow;
}

#endif // SHADOWS_COMMON_HLSLI