//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

Texture2D gDiffuseMap : register(t0);
SamplerState linear_sampler : register(s0);

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	float4x4 gWorldView;
};

struct VertexIn
{
	float3 PosL  : SV_POSITION;
    float4 Color : COLOR;
	float2 Tex : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float2 Tex : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
    vout.Color = vin.Color;
    vout.Tex = vin.Tex;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 color = pin.Color;
	float4 texColor = gDiffuseMap.Sample(linear_sampler, pin.Tex);
	//return color;
	return texColor;
    //return color * texColor;
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
