#include "Common.hlsli"
#include "PBRCommon.hlsli"
#include "ShadowsCommon.hlsli"

// ==========================================================
// TEXTURAS Y SAMPLERS (Sincronizados con InitializeDeferredRootSignature)
// ==========================================================
Texture2D<float4>      g_AlbedoMetal   : register(t0);
Texture2D<float4>      g_NormalRough   : register(t1);
Texture2D<float4>      g_Position      : register(t2);
Texture2DArray<float>  g_ShadowMap     : register(t3, space0);

SamplerComparisonState g_ShadowSampler : register(s0);
SamplerState           g_LinearSampler : register(s1);

struct DirectionalLightData
{
    float3 Direction;
    float  Intensity;
    float3 Color;
    float  Padding;
};

cbuffer DeferredLightingBuffer : register(b0)
{
    DirectionalLightData g_Sun;

    float3               g_GlobalAmbientColor;
    float                g_AmbientPadding;

    float3               g_CameraPosition;
    float                g_CameraPadding;

    float3               g_CameraForward;
    uint                 g_NumCascades;

    float4x4             g_LightSpaceMatrices[MAX_CASCADES];
    float4               g_CascadeSplits;

    uint                 g_FilterQuality;
    float                g_ShadowMapResolution;
    float                g_ShadowLightSize;
    float                g_NormalBiasScale;

    float4               g_CascadeTexelBias;
    float4               g_CascadeWorldTexelSize;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD;
};

// ==========================================================
// VERTEX SHADER (Generación Procedural del Triángulo)
// ==========================================================
PSInput VSMain(uint vertexID : SV_VertexID)
{
    PSInput output;
    output.UV = float2((vertexID << 1) & 2, vertexID & 2);
    output.Position = float4(output.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

// ==========================================================
// PIXEL SHADER (Iluminación Direccional PBR + CSM)
// ==========================================================
float4 PSMain(PSInput input) : SV_TARGET
{
    int3 pixelCoord = int3(input.Position.xy, 0);
    float4 posData  = g_Position.Load(pixelCoord);
    if (posData.w == 0.0f)
    {
        discard;
    }

    float4 albedoMetal = g_AlbedoMetal.Load(pixelCoord);
    float4 normalRough = g_NormalRough.Load(pixelCoord);

    float3 albedo    = albedoMetal.rgb;
    float  metallic  = albedoMetal.a;
    float3 N         = normalize(normalRough.xyz);
    float  roughness = max(normalRough.a, 0.04f);
    float3 worldPos  = posData.xyz;

    float3 V = normalize(g_CameraPosition - worldPos);
    float3 L = normalize(-g_Sun.Direction);

    CSMParams csm;
    csm.LightSpaceMatrices  = g_LightSpaceMatrices;
    csm.CascadeSplits       = g_CascadeSplits;
    csm.NumCascades         = g_NumCascades;
    csm.FilterQuality       = g_FilterQuality;
    csm.ShadowMapResolution = g_ShadowMapResolution;
    csm.ShadowLightSize     = g_ShadowLightSize;
    csm.CascadeTexelBias    = g_CascadeTexelBias;

    float viewDepth    = dot(worldPos - g_CameraPosition, normalize(g_CameraForward));
    uint  cascadeIndex = 0;
    float shadowFactor = CalculateDirectionalShadow(
        g_ShadowMap,
        g_ShadowSampler,
        g_LinearSampler,
        worldPos,
        viewDepth,
        N,
        L,
        input.Position.xy,
        csm,
        cascadeIndex
    );

    float3 F0          = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    float3 sunRadiance = g_Sun.Color * g_Sun.Intensity * shadowFactor;

    float3 directLight = CalculatePBRIllumination(F0, albedo, metallic, roughness, N, V, L, sunRadiance);
    float3 ambient     = g_GlobalAmbientColor * albedo * (1.0f - metallic);

    return float4(ambient + directLight, 1.0f);
}