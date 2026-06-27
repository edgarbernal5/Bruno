#pragma once
#include "Bruno/Math/Math.h"

#include "Bruno/Renderer/Camera.h"
#include "Bruno/Platform/DirectX/IndexBuffer_Gem.h"
#include "Bruno/Platform/DirectX/UploadContext_Gem.h"
#include "Bruno/Platform/DirectX/VertexBuffer_Gem.h"

namespace Bruno::DX
{
    // Estructura ligera para el batching de Gizmos
    struct GizmoVertex
    {
        Math::Vector3 Position;
        Math::Vector4 Color;
    };

    class PrimitiveBatch
    {
    public:
        PrimitiveBatch() = default;
    
        // Prepara las listas para un nuevo frame
        void Begin();
    
        // Primitivas Base (Math puro, generan vértices e índices)
        void DrawLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color);
        void DrawCone(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color);
        void DrawCylinder(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color);
        void DrawBox(const Math::Matrix& transform, float size, const Math::Color& color);
        void DrawTorus(const Math::Matrix& transform, float outerRadius, float innerRadius, int slices, int segments, const Math::Color& color);

        // Finaliza el batching y sube la geometría a los buffers dinámicos de la GPU
        void End(GraphicsDevice* device);

        // Devuelve los buffers para el renderizado
        VertexBuffer* GetVertexBuffer() const { return m_vertexBuffer.get(); }
        IndexBuffer* GetIndexBuffer() const { return m_indexBuffer.get(); }
        uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }

    private:
        uint32_t AddVertex(const Math::Vector3& localPos, const Math::Matrix& transform, const Math::Color& color);
        void AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2);
        
        std::vector<GizmoVertex> m_vertices;
        std::vector<uint32_t> m_indices;

        // Estos buffers deben ser "Dinámicos" o mapeables (creados en un Upload Heap en DX12)
        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::unique_ptr<IndexBuffer> m_indexBuffer;
    };
}
