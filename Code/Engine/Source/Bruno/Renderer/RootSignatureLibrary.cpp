#include "brpch.h"
#include "RootSignatureLibrary.h"

#include "Bruno/Platform/DirectX/RootSignature.h"

namespace Bruno
{
    std::unordered_map<size_t, std::shared_ptr<RootSignature>> RootSignatureLibrary::g_cache;
    std::mutex RootSignatureLibrary::g_mutex;

    std::shared_ptr<RootSignature> RootSignatureLibrary::GetOrCreate(std::shared_ptr<RootSignature> signatureBuilder, RootSignatureFlags flags)
    {
        size_t hash = signatureBuilder->ComputeHash(flags);

        std::lock_guard<std::mutex> lock(g_mutex);

        auto it = g_cache.find(hash);
        if (it != g_cache.end())
        {
            return it->second;
        }

        signatureBuilder->Build(flags);
        
        g_cache[hash] = signatureBuilder;
        return signatureBuilder;
    }

    void RootSignatureLibrary::Clear()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_cache.clear();
    }
}
