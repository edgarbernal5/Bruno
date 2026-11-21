// ==========================================================
// TEXTURAS Y SAMPLERS (Sincronizados con InitializeDeferredRootSignature)
// ==========================================================
Texture2D<float4>      g_AlbedoMetal   : register(t0);
Texture2D<float4>      g_NormalRough   : register(t1);
Texture2D<float4>      g_Position      : register(t2);
Texture2DArray<float>  g_ShadowMap     : register(t3);

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
    uint                 g_ActivePointLightCount;
    uint                 g_ActiveSpotLightCount;
    float3               g_CameraPosition;
    float                g_Padding;
    float3               g_CameraForward;

    float4x4             g_LightSpaceMatrices[MAX_CASCADES];
    float4               g_CascadeSplits;
    uint                 g_NumCascades;

    uint                 g_FilterQuality;
    float                g_ShadowMapResolution;
    float                g_ShadowLightSize;
    float                g_NormalBiasScale;
    float4               g_CascadeTexelBias;
    float4               g_CascadeWorldTexelSize;
};

struct PSInput {
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD;
};

#include "PBRCommon.hlsli"

// ==========================================================
// VERTEX SHADER (Generación Procedural del Triángulo)
// ==========================================================
PSInput VSMain(uint vertexID : SV_VertexID) 
{
    PSInput output;
    
    // Genera un triángulo que excede la pantalla para cubrir todo el rectángulo (NDC)
    // Vértice 0: (-1,  1) -> UV (0, 0) Arriba Izquierda
    // Vértice 1: ( 3,  1) -> UV (2, 0) Arriba Derecha (fuera de pantalla)
    // Vértice 2: (-1, -3) -> UV (0, 2) Abajo Izquierda (fuera de pantalla)
    output.UV = float2((vertexID << 1) & 2, vertexID & 2);
    output.Position = float4(output.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    
    return output;
}

// ==========================================================
// PIXEL SHADER (Iluminación Direccional PBR + CSM)
// ==========================================================
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 posData = g_Position.Sample(g_LinearSampler, input.UV);

    // Descartar píxeles de fondo/cielo (w == 0) para conservar el ClearColor del BackBuffer
    if (posData.w == 0.0f)
    {
        discard;
    }

    float4 albedoMetal = g_AlbedoMetal.Sample(g_LinearSampler, input.UV);
    float4 normalRough = g_NormalRough.Sample(g_LinearSampler, input.UV);

    float3 albedo    = albedoMetal.rgb;
    float  metallic  = albedoMetal.a;
    float3 N         = normalize(normalRough.xyz);
    float  roughness = max(normalRough.a, 0.04f);
    float3 worldPos  = posData.xyz;

    float3 V = normalize(g_CameraPosition - worldPos);
    float3 L = normalize(-g_Sun.Direction);
    float3 H = normalize(V + L);

    // Profundidad planar a lo largo del eje de visión de la cámara
    float viewDepth    = dot(worldPos - g_CameraPosition, normalize(g_CameraForward));
    float shadowFactor = CalculateDirectionalShadow(worldPos, viewDepth, N, L);

    // BRDF Cook-Torrance
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

    float  NDF = DistributionGGX(N, H, roughness);
    float  G   = GeometrySmith(N, V, L, roughness);
    float3 F   = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float3 numerator   = NDF * G * F;
    float  denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
    float3 specular    = numerator / denominator;

    float3 kS = F;
    float3 kD = (1.0f - kS) * (1.0f - metallic);

    float  NdotL    = max(dot(N, L), 0.0f);
    float3 radiance = g_Sun.Color * g_Sun.Intensity;
    float3 Lo       = (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;

    float3 ambient    = g_GlobalAmbientColor * albedo;
    float3 finalColor = ambient + Lo;

    return float4(finalColor, 1.0f);
}