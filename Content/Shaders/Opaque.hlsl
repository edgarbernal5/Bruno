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

struct LightData
{
    float3 Position;
    float Radius;
    float3 Color;
    float Intensity;
};

cbuffer ForwardLights : register(b2) 
{
    LightData g_Lights[MAX_LIGHTS];
    float3 g_AmbientColor;
    uint g_ActiveLightCount;
    
    // Extraído de b0 para la ecuación de Cook-Torrance
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
// PIXEL SHADER (Forward PBR Bindless)
// ==========================================================
float4 PSMain(PixelInput input) : SV_TARGET
{
    // 1. Extraer propiedades del búfer gigante
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];
    
    float4 albedo = mat.AlbedoTint;
    float roughness = max(mat.RoughnessFactor, 0.04f); // Prevenir divisiones por cero
    float metallic = mat.MetallicFactor;
    float3 normal = normalize(input.NormalWorld);
    
    // 2. LECTURA BINDLESS DE TEXTURAS (Albedo y Normal)
    if (mat.AlbedoTextureIndex != 0xFFFFFFFF)
    {
        float4 texColor = g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
        albedo *= texColor;
    }
    
    if (mat.NormalTextureIndex != 0xFFFFFFFF)
    {
        float3 normalSample = g_Textures[NonUniformResourceIndex(mat.NormalTextureIndex)].Sample(g_Sampler, input.UV).xyz;
        normalSample = normalSample * 2.0f - 1.0f;
        
        // Calcular TBN dinámico usando derivadas espaciales (sin necesidad de Tangentes en el VS)
        float3 dp1 = ddx(input.PositionWorld);
        float3 dp2 = ddy(input.PositionWorld);
        float2 duv1 = ddx(input.UV);
        float2 duv2 = ddy(input.UV);
        
        float3 dp2perp = cross(dp2, normal);
        float3 dp1perp = cross(normal, dp1);
        float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
        float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
        
        float invMax = rsqrt(max(dot(T, T), dot(B, B)));
        float3x3 TBN = float3x3(T * invMax, B * invMax, normal);
        normal = normalize(mul(normalSample, TBN));
    }
    
    // 3. VARIABLES PBR GLOBALES
    float3 V = normalize(g_CameraPosition - input.PositionWorld);
    
    // F0 base para plásticos (0.04) o metales (albedo)
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo.rgb, metallic);
    
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    
    // 4. ITERACIÓN DE LUCES (Forward PBR)
    for (uint i = 0; i < g_ActiveLightCount; ++i) 
    {
        LightData light = g_Lights[i];
        
        float3 L = normalize(light.Position - input.PositionWorld);
        float3 H = normalize(V + L);
        
        // Atenuación por distancia (Ley de la inversa del cuadrado) con decaimiento de radio
        float distance = length(light.Position - input.PositionWorld);
        float attenuation = 1.0f / (distance * distance + 0.0001f);
        
        // Decaimiento suave (Windowing) para el límite del radio de la luz
        float distanceByRadius = distance / light.Radius;
        float distanceByRadius4 = distanceByRadius * distanceByRadius * distanceByRadius * distanceByRadius;
        float windowing = clamp(1.0f - distanceByRadius4, 0.0f, 1.0f);
        attenuation *= windowing * windowing;
        
        float3 radiance = light.Color * light.Intensity * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normal, H, roughness);       
        float G   = GeometrySmith(normal, V, L, roughness);      
        float3 F  = FresnelSchlick(max(dot(H, V), 0.0f), F0);
        
        // Conservación de energía
        float3 kS = F;
        float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        kD *= 1.0f - metallic; // Los metales puros no tienen refracción difusa
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0f * max(dot(normal, V), 0.0f) * max(dot(normal, L), 0.0f) + 0.0001f;
        float3 specular = numerator / denominator;
        
        // Acumular radiancia para esta luz
        float NdotL = max(dot(normal, L), 0.0f);
        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    }
    
    // 5. ILUMINACIÓN AMBIENTAL Y RESULTADO FINAL
    float3 ambient = g_AmbientColor * albedo.rgb * 0.1f; // (Idealmente reemplazado por IBL)
    float3 finalColor = ambient + Lo;
    
    // Corrección Gamma (si tu pipeline no usa sRGB render targets nativos)
    // finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f)); // Tone mapping básico (Reinhard)
    // finalColor = pow(finalColor, float3(1.0f/2.2f, 1.0f/2.2f, 1.0f/2.2f)); 
    
    return float4(finalColor, albedo.a);
}