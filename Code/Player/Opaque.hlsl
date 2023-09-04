cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexPosColorTex
{
    float3 Position : POSITION;
    float3 Normal    : NORMAL;
    float2 TexC     : TEXCOORD;
};

struct VertexShaderOutput
{
    float2 TexC    : TEXCOORD;
    float4 Position : SV_Position;
};

VertexShaderOutput main_VS(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.TexC = IN.TexC;

    return OUT;
}

struct PixelShaderInput
{
    float2 TexC    : TEXCOORD;
};

Texture2D    gDiffuseMap : register(t0);
SamplerState gsamLinear  : register(s0);

float4 main_PS( PixelShaderInput IN ) : SV_Target
{
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinear, IN.TexC);
    return diffuseAlbedo;
}