
cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float4 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(vin.PosL, gWorldViewProj);
	
    return vout;
}

technique11 DepthMapTech
{
    pass P00
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( NULL );
    }
}
