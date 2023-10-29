cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    //float4x4 gInverseModelView;
};

struct VertexPosColorTex
{
    float3 Position : POSITION;
    float3 Normal    : NORMAL;
    float2 TexC     : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    //float3 NormalVS   : NORMAL;
    float2 TexC    : TEXCOORD;
};

VertexShaderOutput main_VS(VertexPosColorTex IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(gWorldViewProj, float4(IN.Position, 1.0f));
    OUT.TexC = IN.TexC;
    //OUT.NormalVS = mul((float3x3)gInverseModelView, IN.Normal);
    //OUT.NormalVS = IN.Normal;

    return OUT;
}

Texture2D    gDiffuseMap : register(t0);
SamplerState gsamLinear  : register(s0);

float4 main_PS(VertexShaderOutput IN ) : SV_Target
{
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinear, IN.TexC);
    return diffuseAlbedo;
}