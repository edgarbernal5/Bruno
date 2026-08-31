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
struct VertexInput {
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD;
};

struct PixelInput {
    float4 PositionClip  : SV_POSITION;
    float3 PositionWorld : POSITION;
    float3 NormalWorld   : NORMAL;
    float2 UV            : TEXCOORD;
};

// Mapea exactamente a tu GBuffer (SV_Target0, 1, 2)
struct GBufferOutput {
    float4 AlbedoMetal  : SV_Target0; 
    float4 NormalRough  : SV_Target1; 
    float4 Position     : SV_Target2; 
};

// ==========================================================
// VERTEX SHADER
// ==========================================================
PixelInput VSMain(VertexInput input) {
    PixelInput output;
    
    float4 posWorld = mul(float4(input.Position, 1.0f), g_World);
    output.PositionWorld = posWorld.xyz;
    output.PositionClip = mul(posWorld, g_ViewProjection);
    output.NormalWorld = normalize(mul(input.Normal, (float3x3)g_World));
    output.UV = input.UV;
    
    return output;
}

// ==========================================================
// PIXEL SHADER
// ==========================================================
GBufferOutput PSMain(PixelInput input) {
    GBufferOutput output;
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];
    
    float4 albedo = mat.AlbedoTint;
    if (mat.AlbedoTextureIndex != 0xFFFFFFFF) {
        albedo *= g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
    }
    
    // Empaquetado de datos
    output.AlbedoMetal = float4(albedo.rgb, mat.MetallicFactor);
    output.NormalRough = float4(normalize(input.NormalWorld), mat.RoughnessFactor);
    output.Position    = float4(input.PositionWorld, 1.0f);
    
    return output;
}