cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    //float4x4 gTexTransform;
};

struct VertexPosColorTex
{
    float3 Position : POSITION;
    float3 Color    : COLOR;
    float2 TexC     : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 Color    : COLOR;
    float2 TexC    : TEXCOORD;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.Color = float4(IN.Color, 1.0f);
    OUT.TexC = IN.TexC;

    return OUT;
}