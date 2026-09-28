#pragma once
#include "Bruno/Math/Math.h"

namespace Bruno
{
    // Constante para indicar que no hay textura asignada
    constexpr uint32_t INVALID_TEXTURE_INDEX = 0xFFFFFFFF;

    // ALINEACIÓN ESTRICTA DE 16 BYTES PARA GPU
    __declspec(align(16)) struct MaterialData
    {
        Math::Vector4 AlbedoTint;
        float MetallicFactor;
        float RoughnessFactor;
        uint32_t AlbedoTextureIndex;
        uint32_t NormalTextureIndex;
        
        // Constructor por defecto (Material de plástico blanco básico)
        MaterialData() 
            : AlbedoTint(1.0f, 1.0f, 1.0f, 1.0f), 
              MetallicFactor(0.0f), 
              RoughnessFactor(0.5f),
              AlbedoTextureIndex(INVALID_TEXTURE_INDEX),
              NormalTextureIndex(INVALID_TEXTURE_INDEX) {}
    };
	
}
