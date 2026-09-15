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

struct SpotLight
{
    float3 Position;
    float Radius;
    float3 Direction;
    float Intensity;
    float3 Color;
    float InnerConeCos;
    float OuterConeCos;
    float3 Padding;
};

cbuffer ForwardLights : register(b2)
{
    DirectionalLight g_Sun;
    PointLight g_PointLights[MAX_LIGHTS];
    SpotLight g_SpotLights[MAX_LIGHTS];
    float3 g_AmbientColor;
    uint g_ActivePointLightCount;
    uint g_ActiveSpotLightCount;
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
// MATEMÁTICA PBR (Cook-Torrance)
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

// ----------------------------------------------------------
// FUNCIÓN MAESTRA: Evalúa la luz de cualquier fuente
// ----------------------------------------------------------
float3 CalculatePBRIllumination(float3 F0, float3 albedo, float metallic, float roughness, float3 N, float3 V, float3 L, float3 radiance)
{
    float3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0f);
    float NdotV = max(dot(N, V), 0.0f);
    
    // Evitar divisiones por cero con superficies rasantes
    if (NdotL <= 0.0f) return float3(0.0f, 0.0f, 0.0f);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    float3 F  = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float3 numerator    = NDF * G * F;
    float denominator   = 4.0f * NdotV * NdotL;
    float3 specular     = numerator / max(denominator, 0.0000001f);

    // Conservación de energía: kS (Especular) + kD (Difuso) = 1.0
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic; // Los metales puros absorben la luz difusa

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// ==========================================================
// PIXEL SHADER
// ==========================================================
float4 PSMain(PixelInput input) : SV_TARGET
{
    // 1. Extraer propiedades materiales (Bindless)
    MaterialData mat = g_MaterialBuffer[g_MaterialIndex];
    float4 albedo = mat.AlbedoTint;
    
    if (mat.AlbedoTextureIndex != 0xFFFFFFFF)
    {
        albedo *= g_Textures[NonUniformResourceIndex(mat.AlbedoTextureIndex)].Sample(g_Sampler, input.UV);
    }
    
    // Limitar roughness para evitar divisiones por cero en GGX
    float roughness = max(mat.RoughnessFactor, 0.04f);
    float metallic = mat.MetallicFactor;
    
    float3 N = normalize(input.NormalWorld);
    float3 V = normalize(g_CameraPosition - input.PositionWorld);
    
    // F0: Reflectividad base en ángulo de incidencia 0
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo.rgb, metallic);

    // 2. Luz Ambiental
    float3 finalColor = g_AmbientColor * albedo.rgb * (1.0f - metallic); // Opcional: escalar por Ambient Occlusion si lo tienes

    // 3. ILUMINACIÓN DIRECCIONAL (Sol)
    float3 sunDir = normalize(-g_Sun.Direction);
    float3 sunRadiance = g_Sun.Color * g_Sun.Intensity;
    finalColor += CalculatePBRIllumination(F0, albedo.rgb, metallic, roughness, N, V, sunDir, sunRadiance);

    // 4. LUCES PUNTUALES
    for (uint i = 0; i < g_ActivePointLightCount; ++i)
    {
        float3 lightVec = g_PointLights[i].Position - input.PositionWorld;
        float distance = length(lightVec);
        
        if(distance < g_PointLights[i].Radius)
        {
            float3 lightDir = lightVec / distance;
            
            // Atenuación PBR físicamente correcta (Inversa del Cuadrado con caída suave)
            float distRatio = distance / g_PointLights[i].Radius;
            float attenuation = pow(saturate(1.0f - pow(distRatio, 4.0f)), 2.0f) / (distance * distance + 1.0f);
            
            float3 pointRadiance = g_PointLights[i].Color * g_PointLights[i].Intensity * attenuation;
            finalColor += CalculatePBRIllumination(F0, albedo.rgb, metallic, roughness, N, V, lightDir, pointRadiance);
        }
    }
    
    // 5. LUCES SPOT
    for (uint j = 0; j < g_ActiveSpotLightCount; ++j)
    {
        float3 lightVec = g_SpotLights[j].Position - input.PositionWorld;
        float distance = length(lightVec);
    
        if (distance < g_SpotLights[j].Radius)
        {
            float3 lightDir = lightVec / distance;
        
            // Atenuación PBR de Distancia
            float distRatio = distance / g_SpotLights[j].Radius;
            float distanceAttenuation = pow(saturate(1.0f - pow(distRatio, 4.0f)), 2.0f) / (distance * distance + 1.0f);
        
            // Atenuación Angular (Cono)
            float theta = dot(-lightDir, normalize(g_SpotLights[j].Direction));
            float epsilon = g_SpotLights[j].InnerConeCos - g_SpotLights[j].OuterConeCos;
            float spotAttenuation = saturate((theta - g_SpotLights[j].OuterConeCos) / epsilon);
            spotAttenuation *= spotAttenuation; // Suavizado smoothstep
        
            float3 spotRadiance = g_SpotLights[j].Color * g_SpotLights[j].Intensity * (distanceAttenuation * spotAttenuation);
            finalColor += CalculatePBRIllumination(F0, albedo.rgb, metallic, roughness, N, V, lightDir, spotRadiance);
        }
    }
    
    // HDR Tonemapping (Reinhard) y Gamma Correction (2.2) 
    // Esenciales ya que la luz PBR genera valores superiores a 1.0
    //finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    //finalColor = pow(finalColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    
    return float4(finalColor, albedo.a);
}