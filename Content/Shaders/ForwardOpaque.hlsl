// ==========================================================
// ESTRUCTURAS DE DATOS
// ==========================================================
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct PixelInput
{
    float4 PositionClip : SV_POSITION;
    float3 PositionWorld : POSITION;
    float3 NormalWorld : NORMAL;
    float2 UV : TEXCOORD;
};

struct MaterialData
{
    float4 AlbedoTint;
    float MetallicFactor;
    float RoughnessFactor;
    uint AlbedoTextureIndex;
    uint NormalTextureIndex;
};

// ==========================================================
// ROOT SIGNATURE (Contrato Bindless)
// ==========================================================

// b0: TransformBuffer exclusivo del Vertex Shader
cbuffer TransformBuffer : register(b0)
{
    float4x4 g_World;
    float4x4 g_ViewProjection;
};

// b1: El ID Bindless de 32 bits
cbuffer MaterialConstant : register(b1)
{
    uint g_MaterialIndex;
};

// t0 y t1: El Mega Heap Bindless
StructuredBuffer<MaterialData> g_MaterialBuffer : register(t0);
Texture2D g_Textures[] : register(t1);
SamplerState g_Sampler : register(s0);

// b2: Luces y Cámara para el cálculo PBR del Pixel Shader
#define MAX_LIGHTS 8

struct DirectionalLight
{
    float3 Direction;
    float Intensity;
    float3 Color;
    float Padding;
};

struct PointLight
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
};

cbuffer ForwardLights : register(b2)
{
    DirectionalLight g_Sun;
    PointLight g_Lights[MAX_LIGHTS];
    float3 g_AmbientColor;
    uint g_ActiveLightCount;
    float3 g_CameraPosition;
    float g_Padding;
};

// ==========================================================
// VERTEX SHADER (Clásico y Ligero)
// ==========================================================
PixelInput VSMain(VertexInput input)
{
    PixelInput output;
    
    // 1. Transformación Matemática
    float4 posWorld = mul(float4(input.Position, 1.0f), g_World);
    output.PositionWorld = posWorld.xyz;
    output.PositionClip = mul(posWorld, g_ViewProjection);
    
    // 2. Normal y UVs para el Pixel Shader
    output.NormalWorld = normalize(mul(input.Normal, (float3x3) g_World));
    //output.NormalWorld = input.Normal;
    output.UV = input.UV;
    
    return output;
}

// ==========================================================
// CONSTANTES Y FUNCIONES PBR (GGX / Schlick-GGX)
// ==========================================================
static const float PI = 3.14159265359f;

// Función de Distribución Normal (GGX)
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    
    return num / max(denom, 0.0000001f);
}

// Función de Geometría (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Ecuación de Fresnel (Schlick)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

// ==========================================================
// PIXEL SHADER
// ==========================================================
float4 PSMain(PixelInput input) : SV_TARGET
{
    // 1. Extraer propiedades físicas del búfer gigante (Bindless)
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];
    float4 albedo = mat.AlbedoTint;
    float3 normal = normalize(input.NormalWorld);
    
    if (mat.AlbedoTextureIndex != 0xFFFFFFFF)
    {
        albedo *= g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
    }
    
    float3 viewDir = normalize(g_CameraPosition - input.PositionWorld);
    float3 finalColor = g_AmbientColor * albedo.rgb;

    // 2. ILUMINACIÓN DIRECCIONAL (Fija, sin atenuación de radio)
    float3 sunDir = normalize(-g_Sun.Direction); // Invertir para que apunte hacia el sol
    float sunNdotL = max(dot(normal, sunDir), 0.0f);
    
    // Specular simplificado
    float3 sunHalfVector = normalize(sunDir + viewDir);
    float sunNdotH = max(dot(normal, sunHalfVector), 0.0f);
    float sunSpecPower = exp2(10.0f * (1.0f - mat.RoughnessFactor) + 1.0f);
    float sunSpecular = pow(sunNdotH, sunSpecPower) * (1.0f - mat.RoughnessFactor);
    
    float3 sunSpecColor = lerp(float3(1.0f, 1.0f, 1.0f), albedo.rgb, mat.MetallicFactor) * sunSpecular;
    
    // Sumar el impacto del sol
    finalColor += (albedo.rgb * sunNdotL + sunSpecColor) * g_Sun.Color * g_Sun.Intensity;

    // 3. LUCES PUNTUALES (Iteración sobre el ECS)
    for (uint i = 0; i < g_ActiveLightCount; ++i)
    {
        float3 lightVec = g_Lights[i].Position - input.PositionWorld;
        float distance = length(lightVec);
        
        // Culling matemático: solo iluminar si el píxel está dentro del radio de la luz
        if(distance < g_Lights[i].Radius)
        {
            float3 lightDir = lightVec / distance;
            float attenuation = pow(max(1.0f - (distance / g_Lights[i].Radius), 0.0f), 2.0f); // Atenuación inversa 
            
            float nDotL = max(dot(normal, lightDir), 0.0f);
            
            // ... (Calcular Specular local idéntico al bloque del Sol pero usando lightDir) ...
            // finalColor += (diffuse + specularColor) * g_Lights[i].Color * g_Lights[i].Intensity * attenuation;
        }
    }
    
    
    return float4(finalColor, albedo.a);
}