#pragma once

namespace Bruno
{
    class UploadContext;
    struct MaterialData;

    class MaterialManager
    {
    public:
        MaterialManager(GraphicsDevice& device);
        
        // Crea un nuevo material y devuelve su ID (índice)
        uint32_t CreateMaterial(const MaterialData& data);
        
        // Obtiene la referencia para modificar un material en tiempo real (ej. desde el Editor UI)
        MaterialData& GetMaterial(uint32_t index);
        void MarkAsDirty(); // Llama a esto si modificas un material
        
        // Sincroniza la CPU con la GPU al inicio del frame
        void UpdateGPUBuffer(UploadContext& uploadContext);
        
        // Devuelve el buffer de GPU para bindearlo en el RenderLoop
        ID3D12Resource* GetGPUBuffer() const { return m_gpuBuffer.Get(); }

    private:
        GraphicsDevice& m_device;
        std::vector<MaterialData> m_materials;
        bool m_isDirty = true;

        Microsoft::WRL::ComPtr<ID3D12Resource> m_gpuBuffer;
        uint32_t m_gpuBufferSize = 0;
        
        void ResizeGPUBuffer(uint32_t newElementCount);
    };
}