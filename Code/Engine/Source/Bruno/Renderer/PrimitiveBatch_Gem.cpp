#include "brpch.h"
#include "PrimitiveBatch_Gem.h"

namespace Bruno::DX
{
    void PrimitiveBatch::Begin() {
        // No usamos m_vertices.shrink_to_fit() ni reasignamos para no perder la 
        // capacidad ya reservada en la RAM. Solo reiniciamos el contador.
        m_vertices.clear();
        m_indices.clear();
    }

    // Helper privado para insertar un vértice transformado y retornar su índice
    uint32_t PrimitiveBatch::AddVertex(const Math::Vector3& localPos, const Math::Matrix& transform, const Math::Color& color) {
        GizmoVertex v;
        v.Position = Math::Vector3::Transform(localPos, transform);
        v.Color = color;
    
        uint32_t index = static_cast<uint32_t>(m_vertices.size());
        m_vertices.push_back(v);
        return index;
    }

    // Helper para crear un triángulo (Winding Order Clockwise para DX12)
    void PrimitiveBatch::AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2) {
        m_indices.push_back(i0);
        m_indices.push_back(i1);
        m_indices.push_back(i2);
    }
    
    void PrimitiveBatch::DrawBox(const Math::Matrix& transform, float size, const Math::Color& color) {
        float halfSize = size * 0.5f;

        // 8 vértices del cubo local
        Math::Vector3 corners[8] = {
            Math::Vector3(-halfSize, -halfSize, -halfSize), Math::Vector3(halfSize, -halfSize, -halfSize),
            Math::Vector3(halfSize,  halfSize, -halfSize), Math::Vector3(-halfSize,  halfSize, -halfSize),
            Math::Vector3(-halfSize, -halfSize,  halfSize), Math::Vector3(halfSize, -halfSize,  halfSize),
            Math::Vector3(halfSize,  halfSize,  halfSize), Math::Vector3(-halfSize,  halfSize,  halfSize)
        };

        uint32_t baseIdx = static_cast<uint32_t>(m_vertices.size());
        for (int i = 0; i < 8; ++i) {
            AddVertex(corners[i], transform, color);
        }

        // 12 triángulos (Winding order CW)
        uint32_t indices[] = {
            0,2,1, 0,3,2, // Frente
            1,6,5, 1,2,6, // Derecha
            5,7,4, 5,6,7, // Atrás
            4,3,0, 4,7,3, // Izquierda
            3,6,2, 3,7,6, // Arriba
            4,1,5, 4,0,1  // Abajo
        };

        for (int i = 0; i < 36; ++i) m_indices.push_back(baseIdx + indices[i]);
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

    void PrimitiveBatch::DrawCylinder(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color)
    {
        float halfHeight = height * 0.5f;
    
        // Centros para las tapas
        uint32_t topCenterIdx = AddVertex(Math::Vector3(0, halfHeight, 0), transform, color);
        uint32_t bottomCenterIdx = AddVertex(Math::Vector3(0, -halfHeight, 0), transform, color);

        uint32_t ringBaseIdx = static_cast<uint32_t>(m_vertices.size());

        // Generar vértices de los anillos superior e inferior
        for (int i = 0; i <= slices; ++i) {
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

    void PrimitiveBatch::DrawCone(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color) {
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
    
    void PrimitiveBatch::DrawLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color) {
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
    void PrimitiveBatch::End(GraphicsDevice* device) {
        if (m_vertices.empty() || m_indices.empty()) return;

        size_t vertexBufferSize = m_vertices.size() * sizeof(GizmoVertex);
        size_t indexBufferSize = m_indices.size() * sizeof(uint32_t);

        // 1. Validar y re-alojar Vertex Buffer si no existe o se quedó chico
        if (!m_vertexBuffer || m_vertexBuffer->GetView().SizeInBytes < vertexBufferSize) {
            size_t newSize = static_cast<size_t>(vertexBufferSize * 1.5f);
            // Usamos el constructor dinámico
            m_vertexBuffer = std::make_unique<DX::VertexBuffer>(*device, newSize, sizeof(GizmoVertex),true ); 
        }

        // 2. Validar y re-alojar Index Buffer
        if (!m_indexBuffer || m_indexBuffer->GetView().SizeInBytes < indexBufferSize) {
            size_t newSize = static_cast<size_t>(indexBufferSize * 1.5f);
            // Usamos el constructor dinámico
            m_indexBuffer = std::make_unique<DX::IndexBuffer>(*device, newSize, true);
        }

        // 3. Subir a GPU instántaneamente (Map -> memcpy -> Unmap interno)
        m_vertexBuffer->Update(m_vertices.data(), vertexBufferSize);
        m_indexBuffer->Update(m_indices.data(), indexBufferSize);
    }
}