#pragma once

#include <unordered_map>
#include <memory>

namespace Bruno
{
    struct GraphicsPipelineStateDesc;
    class GraphicsPipelineState;
    class GraphicsDevice;

    class PSOCache
    {
    public:
        static std::shared_ptr<GraphicsPipelineState> GetOrCreate(GraphicsDevice* device, const GraphicsPipelineStateDesc& desc);
        static void Clear();

    private:
        static std::unordered_map<size_t, std::shared_ptr<GraphicsPipelineState>> g_psoMap;
        static std::mutex g_mutex; // Vital si tienes tu Job System creando PSOs en hilos
    };
}