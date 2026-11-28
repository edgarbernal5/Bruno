#include "Common.hlsli"
#include "PBRCommon.hlsli"
#include "ShadowsCommon.hlsli"

// ==========================================================
// ESTRUCTURAS DE DATOS
// ==========================================================
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD;
};

struct PixelInput
{
    float4 PositionClip  : SV_POSITION;
    float3 PositionWorld : POSITION;
    float3 NormalWorld   : NORMAL;
    float2 UV            : TEXCOORD;
};

struct MaterialData
{
    float4 AlbedoTint;
    float  MetallicFactor;
    float  RoughnessFactor;
    uint   AlbedoTextureIndex;
    uint   NormalTextureIndex;
};

// ==========================================================
// ROOT SIGNATURE (Contrato Bindless)
// ==========================================================
cbuffer TransformBuffer : register(b0)
{
    float4x4 g_World;
    float4x4 g_ViewProjection;
};

cbuffer MaterialConstant : register(b1)
{
    uint g_MaterialIndex;
};

// space0: Recursos Bindless y Materiales
StructuredBuffer<MaterialData> g_MaterialBuffer : register(t0, space0);
Texture2D g_Textures[]                          : register(t1, space0);
SamplerState g_Sampler                          : register(s0);

// b2: Luces y Cámara para el cálculo PBR del Pixel Shader
#define MAX_LIGHTS 8

struct DirectionalLight
{
    float3 Direction;
    float  Intensity;
    float3 Color;
    float  Padding;
};

struct PointLight
{
    float3 Position;
    float  Radius;
    float3 Color;
    float  Intensity;
};

struct SpotLight
{
    float3 Position;
    float  Radius;
    float3 Direction;
    float  Intensity;
    float3 Color;
    float  InnerConeCos;
    float  OuterConeCos;
    float3 Padding;
};

cbuffer ForwardLights : register(b2)
{
    DirectionalLight g_Sun;
    PointLight       g_PointLights[MAX_LIGHTS];
    SpotLight        g_SpotLights[MAX_LIGHTS];
    float3           g_AmbientColor;
    uint             g_ActivePointLightCount;
    uint             g_ActiveSpotLightCount;
    float3           g_CameraPosition;
    float            g_Padding;
    float3           g_CameraForward;

    matrix           g_LightSpaceMatrices[MAX_CASCADES];
    float4           g_CascadeSplits;
    uint             g_NumCascades;

    uint             g_FilterQuality;
    float            g_ShadowMapResolution;
    float            g_ShadowLightSize;
    float            g_CsmPadding;          // Mantiene alineación de 16 bytes con ForwardLightingBuffer
    float4           g_CascadeTexelBias;
};

// space1: Recursos específicos de iluminación/sombras
Texture2DArray<float>  g_ShadowMap     : register(t0, space1);
SamplerComparisonState g_ShadowSampler : register(s1);

// ==========================================================
// VERTEX SHADER
// ==========================================================
PixelInput VSMain(VertexInput input)
{
    PixelInput output;

    float4 posWorld = mul(float4(input.Position, 1.0f), g_World);
    output.PositionWorld = posWorld.xyz;
    output.PositionClip  = mul(posWorld, g_ViewProjection);

    output.NormalWorld = normalize(mul(input.Normal, (float3x3)g_World));
    output.UV = input.UV;

    return output;
}

// ==========================================================
// PIXEL SHADER
// ==========================================================
//#define DEBUG_SHOW_CASCADES 1

float4 PSMain(PixelInput input) : SV_TARGET
{
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];
    float4 albedo = mat.AlbedoTint;

    if (mat.AlbedoTextureIndex != 0xFFFFFFFF)
    {
        albedo *= g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
    }

    float roughness = max(mat.RoughnessFactor, 0.04f);
    float metallic  = mat.MetallicFactor;

    float3 N = normalize(input.NormalWorld);
    float3 V = normalize(g_CameraPosition - input.PositionWorld);

    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, metallic);

    // 1. Luz Ambiental
    float3 finalColor = g_AmbientColor * albedo.rgb * (1.0f - metallic);

    // 2. Iluminación Direccional (Sol + CSM)
    float3 sunDir = normalize(-g_Sun.Direction);
    float  viewZ  = dot(input.PositionWorld - g_CameraPosition, g_CameraForward);

    CSMParams csm;
    csm.LightSpaceMatrices  = g_LightSpaceMatrices;
    csm.CascadeSplits       = g_CascadeSplits;
    csm.NumCascades         = g_NumCascades;
    csm.FilterQuality       = g_FilterQuality;
    csm.ShadowMapResolution = g_ShadowMapResolution;
    csm.ShadowLightSize     = g_ShadowLightSize;
    csm.CascadeTexelBias    = g_CascadeTexelBias;

    uint cascadeIndex = 0;
    float shadowFactor = CalculateDirectionalShadow(
        g_ShadowMap,
        g_ShadowSampler,
        g_Sampler,
        input.PositionWorld,
        viewZ,
        N,
        sunDir,
        input.PositionClip.xy,
        csm,
        cascadeIndex
    );

    float3 sunRadiance = g_Sun.Color * g_Sun.Intensity * shadowFactor;
    finalColor += CalculatePBRIllumination(F0, albedo.rgb, metallic, roughness, N, V, sunDir, sunRadiance);

    #if DEBUG_SHOW_CASCADES
        float3 cascadeColors[4] = {
            float3(1.0f, 0.25f, 0.25f),
            float3(0.25f, 1.0f, 0.25f),
            float3(0.25f, 0.5f, 1.0f),
            float3(1.0f, 1.0f, 0.25f)
        };
        uint safeIndex = min(cascadeIndex, 3);
        finalColor = lerp(finalColor, finalColor * cascadeColors[safeIndex] * 2.0f, 0.6f);
    #endif

    return float4(finalColor, albedo.a);
}