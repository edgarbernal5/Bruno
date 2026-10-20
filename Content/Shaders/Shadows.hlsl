// Interleaved Gradient Noise (IGN)
float InterleavedGradientNoise(float2 pixelPos)
{
    float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
    return frac(magic.z * frac(dot(pixelPos, magic.xy)));
}

// Generación de coordenadas polares (Discos de Vogel) para un muestreo distribuido
float2 VogelDiskSample(int sampleIndex, int samplesCount, float noiseAngle)
{
    float r = sqrt((float)sampleIndex + 0.5f) / sqrt((float)samplesCount);
    float theta = sampleIndex * 2.4f + noiseAngle; // 2.4 rad es el ángulo dorado
    return float2(cos(theta), sin(theta)) * r;
}

// --- Hard Shadows (Sin Filtrado PCF) ---
float EvaluateHardShadow(float3 projCoords, uint cascadeIndex, float bias)
{
    // Una sola muestra usando SampleCmpLevelZero. 
    // Como usas un Sampler de Comparación, internamente puede hacer 
    // un pequeño PCF 2x2 bilinear si configuraste el filtro en C++ como 
    // Comparison_MinMag_Linear_MipPoint. 
    // Si en C++ configuraste el filtro como Comparison_Point, será una sombra 100% "pixelada".
    return g_ShadowMap.SampleCmpLevelZero(
        g_ShadowSampler, 
        float3(projCoords.xy, cascadeIndex), 
        projCoords.z - bias
    );
}

// --- PCF 3x3 (Acelerado por Hardware) ---
float EvaluatePCF3x3(float3 projCoords, uint cascadeIndex, float bias, float texelSize)
{
    float shadow = 0.0f;
    float2 offsets[4] = { float2(-0.5f, -0.5f), float2(0.5f, -0.5f), float2(-0.5f, 0.5f), float2(0.5f, 0.5f) };
    
    [unroll]
    for(int i = 0; i < 4; ++i)
    {
        shadow += g_ShadowMap.SampleCmpLevelZero(g_ShadowSampler, float3(projCoords.xy + offsets[i] * texelSize, cascadeIndex), projCoords.z - bias);
    }
    return shadow * 0.25f;
}

// --- PCF 5x5 (Acelerado por Hardware) ---
float EvaluatePCF5x5(float3 projCoords, uint cascadeIndex, float bias, float texelSize)
{
    float shadow = 0.0f;
    [unroll]
    for(int x = -1; x <= 1; ++x)
    {
        [unroll]
        for(int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * 1.5f; // Salto de 1.5 para aprovechar el filtro 2x2 nativo
            shadow += g_ShadowMap.SampleCmpLevelZero(g_ShadowSampler, float3(projCoords.xy + offset * texelSize, cascadeIndex), projCoords.z - bias);
        }
    }
    return shadow / 9.0f;
}

// --- Percentage-Closer Soft Shadows (PCSS) ---
float EvaluatePCSS(float3 projCoords, uint cascadeIndex, float bias, float2 screenPos)
{
    float noiseAngle = InterleavedGradientNoise(screenPos) * 2.0f * PI;

    // 1. Blocker Search (Búsqueda de oclusores)
    int blockerCount = 0;
    float blockerSum = 0.0f;
    
    // Ajustamos el radio de búsqueda basado en la profundidad para que sea anatómicamente correcto
    float searchRadius = g_ShadowLightSize * projCoords.z; 
    int searchSamples = 16;
    
    for(int i = 0; i < searchSamples; ++i)
    {
        float2 offset = VogelDiskSample(i, searchSamples, noiseAngle) * searchRadius;
        
        // Leemos la profundidad usando tu sampler lineal global (s0) ya que la textura es TypeLess
        float depth = g_ShadowMap.SampleLevel(g_Sampler, float3(projCoords.xy + offset, cascadeIndex), 0).r;
        
        if (depth < projCoords.z - bias)
        {
            blockerSum += depth;
            blockerCount++;
        }
    }

    // Retornos tempranos para optimizar
    if (blockerCount == 0) return 1.0f;             // 100% iluminado
    if (blockerCount == searchSamples) return 0.0f; // 100% en sombra

    float avgBlockerDepth = blockerSum / (float)blockerCount;

    // 2. Estimación de Penumbra (Triángulos Semejantes)
    float penumbraRatio = (projCoords.z - avgBlockerDepth) / avgBlockerDepth;
    float filterRadius = penumbraRatio * g_ShadowLightSize;

    // 3. Filtrado (PCF de radio variable)
    float shadow = 0.0f;
    int pcfSamples = 16; // Incrementa a 32 si necesitas más calidad Ultra
    
    for(int j = 0; j < pcfSamples; ++j)
    {
        float2 offset = VogelDiskSample(j, pcfSamples, noiseAngle) * filterRadius;
        shadow += g_ShadowMap.SampleCmpLevelZero(g_ShadowSampler, float3(projCoords.xy + offset, cascadeIndex), projCoords.z - bias);
    }

    return shadow / (float)pcfSamples;
}

float CalculateDirectionalShadow(float3 worldPos, float viewDistance, float3 normal, float3 lightDir, float2 screenPos)
{
    if (g_NumCascades == 0) return 1.0f; 
    
    uint cascadeIndex = 0;
    uint limit = (uint)max(0, (int)g_NumCascades - 1);

    for (uint i = 0; i < limit; ++i)
    {
        if (viewDistance > g_CascadeSplits[i])
        {
            cascadeIndex = i + 1;
        }
    }
    
    float4 lightSpacePos = mul(float4(worldPos, 1.0f), g_LightSpaceMatrices[cascadeIndex]);
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    // Descarte fuera del Frustum de la luz
    if (projCoords.x < 0.0f || projCoords.x > 1.0f || 
        projCoords.y < 0.0f || projCoords.y > 1.0f || 
        projCoords.z < 0.0f || projCoords.z > 1.0f)
    {
        return 1.0f;
    }
    
    // AAA Slope-Scaled Depth Bias: Aumenta el bias si la luz resbala sobre la superficie
    float cosTheta = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    float currentBias = clamp(0.001f * tan(acos(cosTheta)), 0.001f, 0.01f);

    float texelSize = 1.0f / g_ShadowMapResolution;

    // Selección de Calidad de Filtro
    if (g_FilterQuality == 0) // Supongamos que 0 ahora es "None/Hard"
    {
        return EvaluateHardShadow(projCoords, cascadeIndex, currentBias);
    }
    else if (g_FilterQuality == 1) // PCF3x3
    {
        return EvaluatePCF3x3(projCoords, cascadeIndex, currentBias, texelSize);
    }
    else if (g_FilterQuality == 2) // PCF5x5
    {
        return EvaluatePCF5x5(projCoords, cascadeIndex, currentBias, texelSize);
    }
    else // PCSS
    {
        return EvaluatePCSS(projCoords, cascadeIndex, currentBias, screenPos);
    }
}