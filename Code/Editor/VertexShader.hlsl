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

VertexShaderOutput main(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.TexC = IN.TexC;

    return OUT;
}