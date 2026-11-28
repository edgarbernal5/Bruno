// ==========================================================
// ESTRUCTURAS BINDLESS Y CONSTANTES
// ==========================================================
struct MaterialData {
    float4 AlbedoTint;
    float MetallicFactor;
    float RoughnessFactor;
    uint AlbedoTextureIndex;
    uint NormalTextureIndex;
};

cbuffer TransformBuffer : register(b0) {
    float4x4 g_World;
    float4x4 g_ViewProjection;
};

cbuffer MaterialConstant : register(b1) {
    uint g_MaterialIndex;
};

StructuredBuffer<MaterialData> g_MaterialBuffer : register(t0);
Texture2D g_Textures[] : register(t1);
SamplerState g_Sampler : register(s0);

// ==========================================================
// ENTRADAS Y SALIDAS
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

// Mapea exactamente a tu GBuffer (SV_Target0, 1, 2)
struct GBufferOutput
{
    float4 AlbedoMetal : SV_Target0; // RGB = Albedo, A = Metallic
    float4 NormalRough : SV_Target1; // RGB = World Normal, A = Roughness
    float4 Position    : SV_Target2; // RGB = World Position, A = 1.0 (Geometría válida)
};

// Reconstruye la base TBN en espacio de pantalla cuando la malla no incluye Tangentes
float3 PerturbNormal(float3 N, float3 worldPos, float2 uv, float3 mapNormal)
{
    float3 dp1  = ddx(worldPos);
    float3 dp2  = ddy(worldPos);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

    float3 dp2perp = cross(dp2, N);
    float3 dp1perp = cross(N, dp1);
    float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    float invMax = rsqrt(max(dot(T, T), dot(B, B)));
    float3x3 TBN = float3x3(T * invMax, B * invMax, N);

    return normalize(mul(mapNormal, TBN));
}

// ==========================================================
// VERTEX SHADER
// ==========================================================
PixelInput VSMain(VertexInput input)
{
    PixelInput output;

    float4 posWorld      = mul(float4(input.Position, 1.0f), g_World);
    output.PositionWorld = posWorld.xyz;
    output.PositionClip  = mul(posWorld, g_ViewProjection);
    output.NormalWorld   = normalize(mul(input.Normal, (float3x3)g_World));
    output.UV            = input.UV;

    return output;
}

// ==========================================================
// PIXEL SHADER
// ==========================================================
GBufferOutput PSMain(PixelInput input)
{
    GBufferOutput output;
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];

    // 1. Albedo
    float4 albedo = mat.AlbedoTint;
    if (mat.AlbedoTextureIndex != 0xFFFFFFFF)
    {
        albedo *= g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
    }

    // 2. Normal (con soporte automático para Normal Maps)
    float3 normal = normalize(input.NormalWorld);
    if (mat.NormalTextureIndex != 0xFFFFFFFF)
    {
        float3 tangentNormal = g_Textures[NonUniformResourceIndex(mat.NormalTextureIndex)].Sample(g_Sampler, input.UV).xyz * 2.0f - 1.0f;
        normal = PerturbNormal(normal, input.PositionWorld, input.UV, tangentNormal);
    }

    // 3. Empaquetado MRT (w = 1.0f marca píxel ocupado frente al cielo w = 0.0f)
    output.AlbedoMetal = float4(albedo.rgb, mat.MetallicFactor);
    output.NormalRough = float4(normal, max(mat.RoughnessFactor, 0.04f));
    output.Position    = float4(input.PositionWorld, 1.0f);

    return output;
}