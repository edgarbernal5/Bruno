struct GBufferOutput
{
    float4 AlbedoMetal  : SV_Target0;
    float4 NormalRough  : SV_Target1;
    float4 Position     : SV_Target2;
};

GBufferOutput PSMain(PixelInput input)
{
    GBufferOutput output;
    
    // Suponiendo que ya leíste tus texturas Bindless aquí...
    float3 albedo = material.Albedo;
    
    output.AlbedoMetal = float4(albedo, material.Metallic);
    output.NormalRough = float4(normalize(input.NormalWorld), material.Roughness);
    output.Position    = float4(input.PositionWorld, 1.0f);
    
    return output;
}