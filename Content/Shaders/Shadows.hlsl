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

float ComputeCascadeBias(uint cascadeIndex, float3 normal, float3 lightDir)
{
    float unitBias = g_CascadeTexelBias[cascadeIndex];

    // Escala según el alcance del kernel de muestreo activo
    float filterRadiusScale = 1.0f;
    if (g_FilterQuality == 1)      filterRadiusScale = 1.5f; // PCF 3x3
    else if (g_FilterQuality >= 2) filterRadiusScale = 2.5f; // PCF 5x5 y PCSS

    float cosTheta = clamp(dot(normal, lightDir), 0.05f, 1.0f);
    float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float slope = sinTheta / cosTheta; // tan(theta) sin trigonometría

    float baseBias  = unitBias * filterRadiusScale;
    float slopeBias = baseBias * min(slope, 3.0f);

    // Acotamos entre 0.5 y 5 texeles de profundidad propios de esa cascada
    return clamp(baseBias + slopeBias, unitBias * 0.5f, unitBias * 5.0f);
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
// Evalúa una cascada individual y devuelve false si el punto cae fuera de su volumen
bool SampleCascadeFiltered(
    float3 worldPos,
    uint cascadeIndex,
    float bias,
    float texelSize,
    float2 screenPos,
    bool isSecondaryTap,
    out float outShadow)
{
    float4 lightSpacePos = mul(float4(worldPos, 1.0f), g_LightSpaceMatrices[cascadeIndex]);
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f ||
        projCoords.z < 0.0f || projCoords.z > 1.0f)
    {
        outShadow = 1.0f;
        return false;
    }

    // En la cascada secundaria degradamos PCF5x5 (9 taps) o PCSS (32 taps) a PCF3x3 (4 taps)
    uint quality = g_FilterQuality;
    if (isSecondaryTap && quality > 1)
    {
        quality = 1;
    }

    if (quality == 0)
    {
        outShadow = EvaluateHardShadow(projCoords, cascadeIndex, bias);
    }
    else if (quality == 1)
    {
        outShadow = EvaluatePCF3x3(projCoords, cascadeIndex, bias, texelSize);
    }
    else if (quality == 2)
    {
        outShadow = EvaluatePCF5x5(projCoords, cascadeIndex, bias, texelSize);
    }
    else
    {
        outShadow = EvaluatePCSS(projCoords, cascadeIndex, bias, screenPos);
    }

    return true;
}

float CalculateDirectionalShadow(float3 worldPos, float viewDistance, float3 normal, float3 lightDir, float2 screenPos, out uint outCascadeIndex)
{
    if (g_NumCascades == 0)
    {
        outCascadeIndex = 0;
        return 1.0f;
    }

    uint cascadeIndex = 0;
    uint limit = (uint)max(0, (int)g_NumCascades - 1);

    for (uint i = 0; i < limit; ++i)
    {
        if (viewDistance > g_CascadeSplits[i])
        {
            cascadeIndex = i + 1;
        }
    }

    outCascadeIndex = cascadeIndex;

    // Slope-Scaled Depth Bias
    float cosTheta = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    float currentBias = clamp(0.001f * tan(acos(cosTheta)), 0.001f, 0.01f);
    float texelSize = 1.0f / g_ShadowMapResolution;

    // 1. Evaluación de la cascada principal
    float shadow = 1.0f;
    if (!SampleCascadeFiltered(worldPos, cascadeIndex, currentBias, texelSize, screenPos, false, shadow))
    {
        return 1.0f;
    }

    // 2. Cascade Blending en el último 15% de profundidad de la cascada actual
    if (cascadeIndex < limit)
    {
        float splitFar  = g_CascadeSplits[cascadeIndex];
        float splitNear = (cascadeIndex == 0) ? 0.0f : g_CascadeSplits[cascadeIndex - 1];
        float cascadeDepth = max(splitFar - splitNear, 0.001f);

        float blendBand = cascadeDepth * 0.15f;
        float blendFactor = smoothstep(splitFar - blendBand, splitFar, viewDistance);

		/*
		if (blendFactor > InterleavedGradientNoise(screenPos))
    	{
        	cascadeIndex += 1;
    	}
		*/
        [branch]
        if (blendFactor > 0.0f)
        {
            float nextShadow = 1.0f;
            if (SampleCascadeFiltered(worldPos, cascadeIndex + 1, currentBias, texelSize, screenPos, true, nextShadow))
            {
                shadow = lerp(shadow, nextShadow, blendFactor);
            }
        }
    }

    return shadow;
}