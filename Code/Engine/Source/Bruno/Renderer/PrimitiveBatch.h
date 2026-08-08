#pragma once
#include "Bruno/Math/Math.h"

#include "Bruno/Renderer/Camera.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/UploadContext.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"

namespace Bruno
{
    // Estructura ligera para el batching de Gizmos
    struct PrimitiveVertex
    {
        Math::Vector3 Position;
        Math::Vector4 Color;
    };

    class PrimitiveBatch
    {
    public:
        PrimitiveBatch(GraphicsDevice* device);
    
        // Prepara las listas para un nuevo frame
        void Begin();
    
        // Primitivas Base (Math puro, generan vértices e índices)
        void DrawLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color);
        void DrawWireBox(const DirectX::BoundingOrientedBox& obb, const Math::Vector4& color);
        void DrawCone(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color);
        void DrawCylinder(const Math::Matrix& transform, float height, float radius, int slices, const Math::Color& color);
        void DrawBox(const Math::Matrix& transform, float size, const Math::Color& color);
        void DrawBox(const Math::Matrix& transform, const Math::Vector3& size, const Math::Color& color);
        void DrawTorus(const Math::Matrix& transform, float outerRadius, float innerRadius, int slices, int segments, const Math::Color& color);
        void DrawHalfTorus(const Math::Matrix& transform, float outerRadius, float innerRadius, float angleStart, int slices, int segments, const Math::Color& color);
        void DrawSphere(const Math::Matrix& transform, float radius, int slices, int stacks, const Math::Color& color);
        void DrawFrustum(const DirectX::BoundingFrustum& frustum, const Math::Color& color);
        
        // Finaliza el batching y sube la geometría a los buffers dinámicos de la GPU
        void End(uint32_t frameIndex);

        // Devuelve los buffers para el renderizado
        VertexBuffer* GetVertexBuffer(uint32_t frameIndex) const { return m_vertexBuffer[frameIndex].get(); }
        IndexBuffer* GetIndexBuffer(uint32_t frameIndex) const { return m_indexBuffer[frameIndex].get(); }
        uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }
        uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }

    private:
        uint32_t AddVertex(const Math::Vector3& localPos, const Math::Matrix& transform, const Math::Color& color);
        void AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2);
        
        std::vector<PrimitiveVertex> m_vertices;
        std::vector<uint32_t> m_indices;
        GraphicsDevice* m_device;

        // Estos buffers deben ser "Dinámicos" o mapeables (creados en un Upload Heap en DX12)
        std::unique_ptr<VertexBuffer> m_vertexBuffer[2];
        std::unique_ptr<IndexBuffer> m_indexBuffer[2];
    };
}
