// ==========================================
// CONSTANT BUFFERS Y RECURSOS
// ==========================================

// b0: Constant Buffer (Mapea al Root Parameter 0)
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj; // La matriz World * View * Projection combinada
};

// t0: Textura Diffuse/Albedo (Mapea al Root Parameter 1 - Descriptor Table)
Texture2D    gDiffuseMap : register(t0);

// s0: Sampler estático (Definido directamente en la Root Signature)
SamplerState gsamLinear  : register(s0);

// ==========================================
// ESTRUCTURAS DE ENTRADA / SALIDA
// ==========================================

// Debe coincidir con el Input Layout de C++ (tu ModelVertex)
struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;   // Aunque no haya luz, lo declaramos si tu ModelVertex lo tiene
    float2 TexC    : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION; // Posición en Clip Space
    float2 TexC : TEXCOORD;
};

// ==========================================
// VERTEX SHADER
// ==========================================
VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;
    
    // Transformar a espacio homogéneo de recorte (Clip Space)
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    
    // Pasar las UVs directamente al Pixel Shader
    vout.TexC = vin.TexC;
    
    return vout;
}

// ==========================================
// PIXEL SHADER
// ==========================================
float4 PSMain(VertexOut pin) : SV_Target
{
    // Muestrear la textura usando las coordenadas UV
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinear, pin.TexC);
    
    // Retornamos el color de la textura (Alpha = 1.0 para opaco)
    return diffuseAlbedo;
}