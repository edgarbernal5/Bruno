// ==========================================
// ESTRUCTURAS DE DATOS
// ==========================================
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float4 Color    : COLOR;
};

struct PSInput
{
    float4 PositionClip  : SV_POSITION;
    float3 PositionWorld : POSITION;
    float3 Normal        : NORMAL;
    float4 Color         : COLOR;
};

// ==========================================
// CONSTANT BUFFERS
// ==========================================
cbuffer FrameData : register(b0)
{
    matrix ViewProjection;
    float3 CameraPosition;
    float  Padding;
};

// ==========================================
// VERTEX SHADER
// ==========================================
PSInput VSMain(VSInput input)
{
    PSInput output;
    
    // Asumimos que el PrimitiveBatch ya transformó input.Position a World Space en la CPU.
    // (Patrón típico de Dynamic Batching).
    output.PositionClip = mul(float4(input.Position, 1.0f), ViewProjection);
    
    output.PositionWorld = input.Position;
    output.Normal        = input.Normal; 
    output.Color         = input.Color;
    
    return output;
}

// ==========================================
// PIXEL SHADER (LA MAGIA VISUAL)
// ==========================================
float4 PSMain(PSInput input) : SV_TARGET
{
    // Normalizar vectores críticos
    float3 N = normalize(input.Normal);
    float3 V = normalize(CameraPosition - input.PositionWorld);
    
    // ---------------------------------------------------------
    // 1. LUZ DE MINERO (HEADLIGHT) - Iluminación siempre frontal
    // ---------------------------------------------------------
    // Asumimos que la luz principal siempre viene desde la cámara
    float3 L = V; 
    
    // Producto Punto clásico (-1 a 1)
    float dotNL = dot(N, L);
    
    // TRUCO AAA: Half-Lambert
    // En lugar de que las caras traseras sean negras (0.0), mapeamos el rango 
    // de [-1, 1] a [0, 1]. Esto envuelve la luz alrededor del cilindro suavemente.
    float halfLambert = dotNL * 0.5f + 0.5f;
    
    // Interpolar entre un 40% de luz (sombras) y 100% de luz (frente)
    // Esto asegura que el color base siempre se distinga claramente.
    float lightIntensity = lerp(0.4f, 1.0f, halfLambert);
    float3 diffuse = input.Color.rgb * lightIntensity;

    // ---------------------------------------------------------
    // 2. RIM LIGHT (FRESNEL) - Brillo en los bordes
    // ---------------------------------------------------------
    // Calcula qué tan "de perfil" está el píxel respecto a la cámara
    float rim = 1.0f - saturate(dot(N, V));
    
    // Cortamos el efecto para que solo aplique en los bordes extremos
    // smoothstep comprime el gradiente
    rim = smoothstep(0.6f, 1.0f, rim); 
    
    // Elevamos a una potencia para hacer el borde más fino y afilado
    float rimPower = 4.0f;
    float rimIntensity = pow(rim, rimPower);
    
    // El brillo del borde suele ser blanco semitransparente
    float3 rimColor = float3(1.0f, 1.0f, 1.0f) * rimIntensity * 0.5f;

    // ---------------------------------------------------------
    // RESULTADO FINAL
    // ---------------------------------------------------------
    float3 finalRGB = diffuse + rimColor;
    
    // Mantenemos el canal Alpha original (crucial para el Hovering y los Planos Transparentes)
    return float4(finalRGB, input.Color.a);
}