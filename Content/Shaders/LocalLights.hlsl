#include "PBRCommon.hlsli"

struct LightInstanceData
{
    float4x4 World;
    float3   Position;
    float    Radius;
    float3   Color;
    uint     LightType; // 0 = PointLight, 1 = SpotLight
    float3   Direction;
    float    InnerConeCos;
    float    OuterConeCos;
    float    Padding;
};

cbuffer CameraBuffer : register(b0)
{
    float4x4 g_ViewProjection;
    float3   g_CameraPosition;
    float    g_Padding;
};

StructuredBuffer<LightInstanceData> g_LightInstances : register(t0);

Texture2D<float4> g_AlbedoMetal : register(t1);
Texture2D<float4> g_NormalRough : register(t2);
Texture2D<float4> g_Position    : register(t3);
SamplerState      g_LinearSampler : register(s0);

struct VSInput
{
    float3 Position   : POSITION;
    float3 Normal     : NORMAL;
    float2 UV         : TEXCOORD;
    uint   InstanceID : SV_InstanceID;
};

struct PSInput
{
    float4 PosClip    : SV_POSITION;
    float4 PosScreen  : TEXCOORD0;
    nointerpolation uint InstanceID : BLENDINDICES0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4x4 worldMatrix = g_LightInstances[input.InstanceID].World;
    float4   posWorld    = mul(float4(input.Position, 1.0f), worldMatrix);

    output.PosClip    = mul(posWorld, g_ViewProjection);
    output.PosScreen  = output.PosClip;
    output.InstanceID = input.InstanceID;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 ndc      = input.PosScreen.xy / input.PosScreen.w;
    float2 screenUV = ndc * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    float4 posData = g_Position.Sample(g_LinearSampler, screenUV);
    if (posData.w == 0.0f)
    {
        discard;
    }

    float3 worldPos = posData.xyz;
    LightInstanceData light = g_LightInstances[input.InstanceID];

    float3 lightVec = light.Position - worldPos;
    float  distance = length(lightVec);
    if (distance >= light.Radius)
    {
        discard;
    }

    float3 L = lightVec / max(distance, 0.0001f);

    float attenuation = CalculateDistanceAttenuation(distance, light.Radius);
    if (light.LightType == 1)
    {
        attenuation *= CalculateSpotAttenuation(L, light.Direction, light.InnerConeCos, light.OuterConeCos);
    }

    if (attenuation <= 0.0001f)
    {
        discard;
    }

    float4 albedoMetal = g_AlbedoMetal.Sample(g_LinearSampler, screenUV);
    float4 normalRough = g_NormalRough.Sample(g_LinearSampler, screenUV);

    float3 albedo    = albedoMetal.rgb;
    float  metallic  = albedoMetal.a;
    float3 N         = normalize(normalRough.xyz);
    float  roughness = max(normalRough.a, 0.04f);

    float3 V        = normalize(g_CameraPosition - worldPos);
    float3 F0       = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    float3 radiance = light.Color * attenuation;

    float3 directLight = CalculatePBRIllumination(F0, albedo, metallic, roughness, N, V, L, radiance);
    return float4(directLight, 1.0f);
}