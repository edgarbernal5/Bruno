#include "brpch.h"
#include "PrimitiveBatch_Gem.h"

namespace Bruno::DX
{
    PrimitiveBatch::PrimitiveBatch(GraphicsDevice* device) : m_device(device)
    {
        // Alojar suficiente espacio para unos 65,000 vértices de gizmos (aprox 1.5 MB)
        // Así nunca se disparará el if de redimensionamiento.
        size_t initialVertexCapacity = 65536 * sizeof(GizmoVertex);
        size_t initialIndexCapacity = 65536 * sizeof(uint32_t);
        
        for (int i = 0; i < 2; ++i)
        {
            m_vertexBuffer[i] = std::make_unique<DX::VertexBuffer>(*device, initialVertexCapacity, sizeof(GizmoVertex), true);
            m_indexBuffer[i] = std::make_unique<DX::IndexBuffer>(*device, initialIndexCapacity, true);
        }
    }

    void PrimitiveBatch::Begin()
    {
        // No usamos m_vertices.shrink_to_fit() ni reasignamos para no perder la 
        // capacidad ya reservada en la RAM. Solo reiniciamos el contador.
        m_vertices.clear();
        m_indices.clear();
    }

    // Helper privado para insertar un vértice transformado y retornar su índice
    uint32_t PrimitiveBatch::AddVertex(const Math::Vector3& localPos, const Math::Matrix& transform, const Math::Color& color)
    {
        GizmoVertex v;
        v.Position = Math::Vector3::Transform(localPos, transform);
        v.Color = color;
    
        uint32_t index = static_cast<uint32_t>(m_vertices.size());
        m_vertices.push_back(v);
        return index;
    }

    // Helper para crear un triángulo (Winding Order Clockwise para DX12)
    void PrimitiveBatch::AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2)
    {
        m_indices.push_back(i0);
        m_indices.push_back(i1);
        m_indices.push_back(i2);
    }
    
    // Si necesitas cajas rectangulares
    void PrimitiveBatch::DrawBox(const Math::Matrix& transform, const Math::Vector3& size, const Math::Color& color)
    {
        uint32_t baseIdx = static_cast<uint32_t>(m_vertices.size());
        Math::Vector3 halfSize = size * 0.5f;

        // 1. Generar los 8 vértices únicos de un cubo
        // Orden lógico: Bottom/Top, Left/Right, Front/Back
        AddVertex(Math::Vector3(-halfSize.x, -halfSize.y, -halfSize.z), transform, color); // 0: Izquierda-Abajo-Frente
        AddVertex(Math::Vector3( halfSize.x, -halfSize.y, -halfSize.z), transform, color); // 1: Derecha-Abajo-Frente
        AddVertex(Math::Vector3( halfSize.x,  halfSize.y, -halfSize.z), transform, color); // 2: Derecha-Arriba-Frente
        AddVertex(Math::Vector3(-halfSize.x,  halfSize.y, -halfSize.z), transform, color); // 3: Izquierda-Arriba-Frente
    
        AddVertex(Math::Vector3(-halfSize.x, -halfSize.y,  halfSize.z), transform, color); // 4: Izquierda-Abajo-Fondo
        AddVertex(Math::Vector3( halfSize.x, -halfSize.y,  halfSize.z), transform, color); // 5: Derecha-Abajo-Fondo
        AddVertex(Math::Vector3( halfSize.x,  halfSize.y,  halfSize.z), transform, color); // 6: Derecha-Arriba-Fondo
        AddVertex(Math::Vector3(-halfSize.x,  halfSize.y,  halfSize.z), transform, color); // 7: Izquierda-Arriba-Fondo

        // 2. Generar los Índices (36 índices, 2 triángulos por cada una de las 6 caras)
        // Winding Order: Clockwise (Sentido Horario) para DirectX

        // Cara Frontal (-Z)
        AddTriangle(baseIdx + 0, baseIdx + 3, baseIdx + 1);
        AddTriangle(baseIdx + 1, baseIdx + 3, baseIdx + 2);

        // Cara Trasera (+Z)
        AddTriangle(baseIdx + 5, baseIdx + 6, baseIdx + 4);
        AddTriangle(baseIdx + 4, baseIdx + 6, baseIdx + 7);

        // Cara Superior (+Y)
        AddTriangle(baseIdx + 3, baseIdx + 7, baseIdx + 2);
        AddTriangle(baseIdx + 2, baseIdx + 7, baseIdx + 6);

        // Cara Inferior (-Y)
        AddTriangle(baseIdx + 4, baseIdx + 0, baseIdx + 5);
        AddTriangle(baseIdx + 5, baseIdx + 0, baseIdx + 1);

        // Cara Izquierda (-X)
        AddTriangle(baseIdx + 4, baseIdx + 7, baseIdx + 0);
        AddTriangle(baseIdx + 0, baseIdx + 7, baseIdx + 3);

        // Cara Derecha (+X)
        AddTriangle(baseIdx + 1, baseIdx + 2, baseIdx + 5);
        AddTriangle(baseIdx + 5, baseIdx + 2, baseIdx + 6);
    }

    void PrimitiveBatch::DrawTorus(const Math::Matrix& transform, float outerRadius, float innerRadius, int slices, int segments, const Math::Color& color)
    {
        // baseIdx nos sirve para saber dónde empiezan nuestros vértices dentro del buffer global
        uint32_t baseIdx = static_cast<uint32_t>(m_vertices.size());

        // 1. Generar Vértices
        // segments: subdivisiones a lo largo del anillo principal (ej: 32 o 48)
        // slices: subdivisiones del tubo que forma el anillo (ej: 8 o 12)
        for (int i = 0; i < segments; ++i) {
            float theta = (static_cast<float>(i) / segments) * Math::TWO_PI; // Ángulo del anillo principal
            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            for (int j = 0; j < slices; ++j) {
                float phi = (static_cast<float>(j) / slices) * Math::TWO_PI; // Ángulo del tubo
                float cosPhi = cosf(phi);
                float sinPhi = sinf(phi);

                // Ecuación paramétrica del toroide (orientado en el plano XZ por defecto)
                // X e Z forman el círculo principal, Y da la altura del tubo
                Math::Vector3 localPos(
                    (outerRadius + innerRadius * cosPhi) * cosTheta,
                    innerRadius * sinPhi,
                    (outerRadius + innerRadius * cosPhi) * sinTheta
                );

                AddVertex(localPos, transform, color);
            }
        }

        // 2. Generar Índices (Triángulos)
        for (int i = 0; i < segments; ++i)
        {
            // Conectamos el último segmento con el primero usando módulo (%)
            int nextI = (i + 1) % segments; 

            for (int j = 0; j < slices; ++j) {
                // Conectamos la última cara del tubo con la primera
                int nextJ = (j + 1) % slices;

                // Índices de los 4 vértices que forman un quad en la superficie del toroide
                uint32_t a = baseIdx + (i * slices + j);
                uint32_t b = baseIdx + (nextI * slices + j);
                uint32_t c = baseIdx + (i * slices + nextJ);
                uint32_t d = baseIdx + (nextI * slices + nextJ);

                // Añadimos los dos triángulos que forman el quad.
                // Winding Order: Clockwise (CW) para DirectX 12.
                AddTriangle(a, c, b);
                AddTriangle(b, c, d);
            }
        }
    }

    void PrimitiveBatch::DrawHalfTorus(const Math::Matrix& transform, float outerRadius, float innerRadius, float angleStart, int slices, int segments, const Math::Color& color)
    {
        uint32_t baseIdx = static_cast<uint32_t>(m_vertices.size());
    
        // 1. Generar Vértices
        // Nota el "<= segments" y "<= slices". Queremos cerrar el tubo en sí mismo (slices),
        // pero dejar el arco abierto en los extremos (segments).
        for (int i = 0; i <= segments; ++i)
        { 
            // Medio círculo: de angleStart a angleStart + PI
            float theta = angleStart + (static_cast<float>(i) / segments) * Math::PI; 
            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            for (int j = 0; j <= slices; ++j)
            { 
                // Anillo del tubo completo (el grosor): de 0 a 2*PI
                float phi = (static_cast<float>(j) / slices) * (Math::PI * 2.0f); 
                float cosPhi = cosf(phi);
                float sinPhi = sinf(phi);

                Math::Vector3 localPos(
                    (outerRadius + innerRadius * cosPhi) * cosTheta,
                    innerRadius * sinPhi,
                    (outerRadius + innerRadius * cosPhi) * sinTheta
                );

                AddVertex(localPos, transform, color);
            }
        }

        // 2. Generar Índices
        int stride = slices + 1; // Cuántos vértices hay en un anillo transversal
        for (int i = 0; i < segments; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                uint32_t a = baseIdx + (i * stride + j);
                uint32_t b = baseIdx + ((i + 1) * stride + j);
                uint32_t c = baseIdx + (i * stride + j + 1);
                uint32_t d = baseIdx + ((i + 1) * stride + j + 1);

                // Winding order Clockwise
                AddTriangle(a, c, b);
                AddTriangle(b, c, d);
            }
        }
    }

    void PrimitiveBatch::DrawSphere(const Math::Matrix& transform, float radius, int slices, int stacks, const Math::Color& color)
    {
        // Protecciones básicas
        if (slices < 3) slices = 3;
        if (stacks < 2) stacks = 2;

        // Vector temporal para guardar los índices reales (globales) que nos devuelve AddVertex
        std::vector<uint32_t> vertexIndices;
        vertexIndices.reserve((stacks + 1) * (slices + 1));

        // ==========================================
        // 1. GENERAR VÉRTICES
        // ==========================================
        for (int i = 0; i <= stacks; ++i)
        {
            // V va de 0.0 (polo norte) a 1.0 (polo sur)
            float v = static_cast<float>(i) / stacks;
            float phi = v * Math::PI; // Ángulo vertical (0 a PI)

            for (int j = 0; j <= slices; ++j)
            {
                // U va de 0.0 a 1.0 alrededor del ecuador
                float u = static_cast<float>(j) / slices;
                float theta = u * Math::PI * 2.0f; // Ángulo horizontal (0 a 2PI)

                // Coordenadas esféricas a cartesianas locales
                float x = radius * std::sin(phi) * std::cos(theta);
                float y = radius * std::cos(phi);
                float z = radius * std::sin(phi) * std::sin(theta);

                Math::Vector3 localPos(x, y, z);
            
                // Usamos tu helper maravilloso: él se encarga de transformarlo y añadirlo
                uint32_t realIndex = AddVertex(localPos, transform, color);
                vertexIndices.push_back(realIndex);
            }
        }

        // ==========================================
        // 2. GENERAR ÍNDICES (Winding Order Clockwise)
        // ==========================================
        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                int nextI = i + 1;
                int nextJ = j + 1;

                // Mapeamos nuestra grilla 2D al índice real devuelto por AddVertex
                uint32_t p0 = vertexIndices[i * (slices + 1) + j];         // Arriba-Izquierda
                uint32_t p1 = vertexIndices[nextI * (slices + 1) + j];     // Abajo-Izquierda
                uint32_t p2 = vertexIndices[nextI * (slices + 1) + nextJ]; // Abajo-Derecha
                uint32_t p3 = vertexIndices[i * (slices + 1) + nextJ];     // Arriba-Derecha

                // Triángulo 1: Arriba-Izquierda -> Arriba-Derecha -> Abajo-Derecha (CW)
                AddTriangle(p0, p3, p2);

                // Triángulo 2: Arriba-Izquierda -> Abajo-Derecha -> Abajo-Izquierda (CW)
                AddTriangle(p0, p2, p1);
            }
        }
    }

    void PrimitiveBatch::DrawCylinder(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color)
    {
        float halfHeight = height * 0.5f;
    
        // Centros para las tapas
        uint32_t topCenterIdx = AddVertex(Math::Vector3(0, halfHeight, 0), transform, color);
        uint32_t bottomCenterIdx = AddVertex(Math::Vector3(0, -halfHeight, 0), transform, color);

        uint32_t ringBaseIdx = static_cast<uint32_t>(m_vertices.size());

        // Generar vértices de los anillos superior e inferior
        for (int i = 0; i <= slices; ++i)
        {
            float theta = (static_cast<float>(i) / slices) * Math::TWO_PI;
            float x = cosf(theta) * radius;
            float z = sinf(theta) * radius;

            AddVertex(Math::Vector3(x, halfHeight, z), transform, color); // Arriba
            AddVertex(Math::Vector3(x, -halfHeight, z), transform, color); // Abajo
        }

        // Unir los vértices con triángulos
        for (int i = 0; i < slices; ++i) {
            uint32_t top1 = ringBaseIdx + (i * 2);
            uint32_t bot1 = top1 + 1;
            uint32_t top2 = ringBaseIdx + ((i + 1) * 2);
            uint32_t bot2 = top2 + 1;

            // Pared del cilindro
            AddTriangle(top1, top2, bot1);
            AddTriangle(bot1, top2, bot2);

            // Tapas
            AddTriangle(topCenterIdx, top1, top2);
            AddTriangle(bottomCenterIdx, bot2, bot1);
        }
    }

    // Sobrecarga cómoda para cubos perfectos (como los que usamos en las puntas del Scale Gizmo)
    void PrimitiveBatch::DrawBox(const Math::Matrix& transform, float size, const Math::Color& color)
    {
        DrawBox(transform, Math::Vector3(size, size, size), color);
    }

    void PrimitiveBatch::DrawCone(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color)
    {
        // El origen del cono para un gizmo suele ser su base, apuntando hacia arriba (+Y)
        uint32_t apexIdx = AddVertex(Math::Vector3(0, height, 0), transform, color);
        uint32_t baseCenterIdx = AddVertex(Math::Vector3(0, 0, 0), transform, color);

        uint32_t ringBaseIdx = static_cast<uint32_t>(m_vertices.size());

        for (int i = 0; i <= slices; ++i) {
            float theta = (static_cast<float>(i) / slices) * Math::TWO_PI;
            float x = cosf(theta) * radius;
            float z = sinf(theta) * radius;
            AddVertex(Math::Vector3(x, 0, z), transform, color);
        }

        for (int i = 0; i < slices; ++i) {
            uint32_t v1 = ringBaseIdx + i;
            uint32_t v2 = ringBaseIdx + i + 1;

            // Pared del cono
            AddTriangle(apexIdx, v2, v1);
            // Tapa base
            AddTriangle(baseCenterIdx, v1, v2);
        }
    }
    
    void PrimitiveBatch::DrawLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color)
    {
        // Calculamos la dirección y longitud
        Math::Vector3 dir = end - start;
        float length = dir.Length();
        if (length < 0.0001f) return;
    
        dir.Normalize();

        // Creamos una matriz que apunte hacia esa dirección y la escalamos
        // Utilizamos CreateLookAt o utilidades similares de math.
        // En DXMath: XMMatrixLookToLH o crear un quaternion desde un vector UP hacia 'dir'.
        Math::Matrix lineTransform = Math::Matrix::CreateWorld(start, dir, Math::Vector3::Up);
    
        // Convertimos la línea en un rectángulo muy fino (Grosor: 0.02f)
        // Desplazamos el centro porque DrawBox lo centra, pero queremos que inicie en 'start'
        Math::Matrix offset = Math::Matrix::CreateTranslation(0, 0, length * 0.5f); 
    
        // Escala no uniforme: Ancho(X), Alto(Y), Largo(Z)
        Math::Matrix scale = Math::Matrix::CreateScale(0.02f, 0.02f, length);
    
        DrawBox(scale * offset * lineTransform, 1.0f, color);
    }
    
    // Fíjate que ya no pasamos el UploadContext como parámetro
    void PrimitiveBatch::End(uint32_t frameIndex)
    {
        if (m_vertices.empty() || m_indices.empty())
        {
            return;
        }
        
        size_t vertexBufferSize = m_vertices.size() * sizeof(GizmoVertex);
        size_t indexBufferSize = m_indices.size() * sizeof(uint32_t);

        // 1. Validar y re-alojar Vertex Buffer si no existe o se quedó chico
        if (!m_vertexBuffer || m_vertexBuffer[frameIndex]->GetView().SizeInBytes < vertexBufferSize)
        {
        }

        // 2. Validar y re-alojar Index Buffer
        if (!m_indexBuffer || m_indexBuffer[frameIndex]->GetView().SizeInBytes < indexBufferSize)
        {
        }

        // 3. Subir a GPU instántaneamente (Map -> memcpy -> Unmap interno)
        m_vertexBuffer[frameIndex]->Update(m_vertices.data(), vertexBufferSize);
        m_indexBuffer[frameIndex]->Update(m_indices.data(), indexBufferSize);
    }
}