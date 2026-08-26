// ==========================================================
// TEXTURAS DEL G-BUFFER (Fase de Lectura)
// ==========================================================
Texture2D<float4> g_AlbedoMetal  : register(t0); 
Texture2D<float4> g_NormalRough  : register(t1); 
Texture2D<float4> g_Position     : register(t2); 

SamplerState g_LinearSampler : register(s0);

cbuffer LightData : register(b0) {
    float3 g_LightDir;
    float  g_Padding1;
    float3 g_LightColor;
    float  g_Padding2;
    float3 g_CameraPos;
    float  g_Padding3;
};

struct PSInput {
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD;
};

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
// PIXEL SHADER (Iluminación)
// ==========================================================
float4 PSMain(PSInput input) : SV_TARGET
{
    // 1. Leer G-Buffer
    float4 albedoMetal = g_AlbedoMetal.Sample(g_LinearSampler, input.UV);
    float4 normalRough = g_NormalRough.Sample(g_LinearSampler, input.UV);
    float4 posData     = g_Position.Sample(g_LinearSampler, input.UV);
    
    float3 albedo   = albedoMetal.rgb;
    float metallic  = albedoMetal.a;
    float3 normal   = normalize(normalRough.xyz);
    float roughness = normalRough.a;
    float3 worldPos = posData.xyz;
    
    // 2. Cálculos PBR base
    float3 viewDir = normalize(g_CameraPos - worldPos);
    float3 lightDir = normalize(-g_LightDir);
    
    // Ecuación difusa simple (reemplazar con Cook-Torrance completo después)
    float nDotL = max(dot(normal, lightDir), 0.0);
    float3 diffuse = albedo * nDotL * g_LightColor;
    
    float3 ambient = albedo * 0.05f;
    float3 finalColor = ambient + diffuse;
    
    // (Opcional) Aquí aplicarías tu SampleCmpLevelZero para las sombras en cascada[cite: 7]
    
    return float4(finalColor, 1.0f);
}