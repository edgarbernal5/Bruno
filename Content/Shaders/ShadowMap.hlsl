// ==============================================================================
// Shadow Map Pass (Cascaded Shadow Maps)
// ==============================================================================

// Constant Buffer para las matrices (Enlazado en el registro b0)
cbuffer ShadowConstants : register(b0)
{
    float4x4 g_World;          // Matriz de mundo de la entidad actual
    float4x4 g_LightViewProj;  // Matriz ortográfica de la cascada actual
};

struct VS_INPUT
{
    float3 Position : POSITION;
    // No necesitamos Normales, UVs ni Tangentes aquí. 
    // Ignorarlas en el Input Layout del PSO ahorra ancho de banda de memoria.
};

// ==============================================================================
// VERTEX SHADER
// ==============================================================================
float4 VSMain(VS_INPUT input) : SV_POSITION
{
    // 1. Llevar el vértice al espacio del mundo
    float4 worldPos = mul(float4(input.Position, 1.0f), g_World);
    
    // 2. Proyectar desde el mundo hacia el punto de vista del sol (Cascada)
    float4 lightSpacePos = mul(worldPos, g_LightViewProj);
    
    return lightSpacePos;
}

// ==============================================================================
// PIXEL SHADER (NULO / VACÍO)
// ==============================================================================
// No hay función PSMain. El pipeline de DirectX 12 desactivará el Pixel Shader.
// La GPU rasterizará SV_POSITION y escribirá directamente en el Texture2DArray.