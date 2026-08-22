#include "brpch.h"
#include "PSOCache.h"

#include "Bruno/Platform/DirectX/GraphicsPipelineState.h"

namespace Bruno
{
    std::unordered_map<size_t, std::shared_ptr<GraphicsPipelineState>> PSOCache::g_psoMap;
    std::mutex PSOCache::g_mutex;

    std::shared_ptr<GraphicsPipelineState> PSOCache::GetOrCreate(GraphicsDevice* device, const GraphicsPipelineStateDesc& desc)
    {
        size_t hash = desc.ComputeHash();

        std::lock_guard<std::mutex> lock(g_mutex);

        auto it = g_psoMap.find(hash);
        if (it != g_psoMap.end())
        {
            return it->second;
        }

        auto newPSO = std::make_shared<GraphicsPipelineState>(*device);
        newPSO->Initialize(desc);
        
        g_psoMap[hash] = newPSO;
        
        return newPSO;
    }

    void PSOCache::Clear()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_psoMap.clear();
    }
}
