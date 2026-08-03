// Constant Buffer para el rectángulo
cbuffer MarqueeCB : register(b0)
{
    float2 RectMin; // Coordenadas en NDC [-1, 1]
    float2 RectMax; // Coordenadas en NDC [-1, 1]
    float4 FillColor;
    float4 BorderColor;
    float BorderThickness; // Porcentaje (ej. 0.02)
    float padding[3];
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV  : TEXCOORD0;
};

// Generamos el quad sin Vertex Buffer (Dibujamos usando un Triangle Strip de 4 vértices)
VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    // UVs de las 4 esquinas
    float2 texCoords[4] = { float2(0,0), float2(1,0), float2(0,1), float2(1,1) };
    
    // Posiciones a partir de RectMin y RectMax
    float2 pos[4] = {
        RectMin,
        float2(RectMax.x, RectMin.y),
        float2(RectMin.x, RectMax.y),
        RectMax
    };

    output.Pos = float4(pos[vertexID], 0.0f, 1.0f); // Z = 0, W = 1
    output.UV = texCoords[vertexID];
    
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    // Lógica para dibujar el borde usando UVs
    if (input.UV.x < BorderThickness || input.UV.x > 1.0f - BorderThickness ||
        input.UV.y < BorderThickness || input.UV.y > 1.0f - BorderThickness)
    {
        return BorderColor;
    }
    
    return FillColor;
}