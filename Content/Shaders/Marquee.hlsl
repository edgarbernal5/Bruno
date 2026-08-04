cbuffer MarqueeData : register(b0)
{
    float2 RectMin;
    float2 RectMax;
    float4 FillColor;
    float4 BorderColor;
    float BorderThicknessX;
    float BorderThicknessY;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 PosNDC : TEXCOORD0; // <--- NUEVO: Guardamos la posición absoluta NDC
};

VSOutput VS(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    float2 pos = float2(
        (vertexID & 1) ? RectMax.x : RectMin.x,
        (vertexID & 2) ? RectMax.y : RectMin.y
    );

    output.Position = float4(pos, 0.0f, 1.0f);
    output.PosNDC = pos; // <--- Pasamos la coordenada NDC tal cual al Pixel Shader
    
    return output;
}

float4 PS(VSOutput input) : SV_TARGET
{
    // 1. Identificar los límites reales del rectángulo
    // (min/max protegen el cálculo si el usuario arrastra de derecha a izquierda)
    float leftEdge   = min(RectMin.x, RectMax.x);
    float rightEdge  = max(RectMin.x, RectMax.x);
    float topEdge    = min(RectMin.y, RectMax.y);
    float bottomEdge = max(RectMin.y, RectMax.y);

    // 2. Calcular la distancia absoluta del píxel actual a los 4 bordes
    float distLeft   = abs(input.PosNDC.x - leftEdge);
    float distRight  = abs(input.PosNDC.x - rightEdge);
    float distTop    = abs(input.PosNDC.y - topEdge);
    float distBottom = abs(input.PosNDC.y - bottomEdge);

    // 3. Si el píxel está más cerca de CUALQUIER borde que nuestro grosor, es borde.
    if (distLeft < BorderThicknessX || distRight < BorderThicknessX ||
        distTop < BorderThicknessY || distBottom < BorderThicknessY)
    {
        return BorderColor;
    }
    
    return FillColor;
}