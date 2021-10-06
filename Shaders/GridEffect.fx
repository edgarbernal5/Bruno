
cbuffer cbPerObject //: register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorldView;
};

cbuffer cbPerScene //: register(b1)
{
	float gNearPlane;
	float gFarPlane;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
    float4 PosView : TEXCOORD0;
    float4 PosViewProj : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosView = mul(float4(vin.PosL, 1.0f), gWorldView);
	vout.PosViewProj = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
    vout.Color = vin.Color;
    
    return vout;
}

/*
ProjRatio .xy= float2 ( zfar /( zfar - znear ), znear /( znear - zfar ));
float GetLinearDepth ( float fDevDepth )
{
return ProjRatio .y/( fDevDepth - ProjRatio .x);
}
*/

float4 PS(VertexOut pin) : SV_Target
{
	float2 projRatio = float2 ( gFarPlane /( gFarPlane - gNearPlane ), gNearPlane /( gNearPlane - gFarPlane ));
	float depth = pin.PosViewProj.z / pin.PosViewProj.w;
	float depthSC = pin.PosView.z / pin.PosView.w;
	float linearDepthSC = depthSC / gFarPlane;
	//float lineardepth = (2.0f * gNearPlane) / (gFarPlane + gNearPlane - depth * (gFarPlane - gNearPlane));
	float lineardepth = projRatio.y/( depth - projRatio.x);
	
	float4 outputColor = pin.Color;
	outputColor.a = lerp(0.5, 0.2, lineardepth);
	//outputColor.xyz = gNearPlane;
	return outputColor;
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
