cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexPosColorTex
{
    float3 Position : POSITION;
    float3 Normal    : NORMAL;
    float4 Color     : COLOR0;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float4 Color     : COLOR0;
};

VertexShaderOutput main_VS(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.Color = IN.Color;

    return OUT;
}

float4 main_PS(VertexShaderOutput IN ) : SV_Target
{
    return IN.Color;
}