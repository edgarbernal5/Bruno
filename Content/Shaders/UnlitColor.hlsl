cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VertexPosColorTex
{
    float3 Position : POSITION;
    float4 Color     : COLOR0;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float4 Color     : COLOR0;
};

VertexShaderOutput VSMain(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.Color = IN.Color;

    return OUT;
}

float4 PSMain(VertexShaderOutput IN) : SV_Target
{
    if (IN.Color.a < 0.1f)
    {
        discard; // O clip(IN.Color.a - 0.1f);
    }
    return IN.Color;
}