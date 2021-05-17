
cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Textura : TEXCOORD0;
};

Texture2D gDiffuseMap : register(t0);
SamplerState linear_sampler : register(s0);

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
    float2 Textura : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	//vout.PosH = mul(gWorldViewProj, float4(vin.PosL, 1.0f));
	
    vout.Color = float4(1,1,1,1);
    vout.Textura = vin.Textura;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 texColor = gDiffuseMap.Sample(linear_sampler, pin.Textura);
	return texColor;
}

technique11 ColorTech
{
    pass P00
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
