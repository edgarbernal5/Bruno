#pragma once

#include <memory>

#include "RHITypes.h"

namespace Bruno
{
    class RootSignature;

    class RootSignatureLibrary
    {
    public:
        static std::shared_ptr<RootSignature> GetOrCreate(std::shared_ptr<RootSignature> signatureBuilder, RootSignatureFlags flags = RootSignatureFlags::AllowInputAssembler);
        static void Clear();

    private:
        static std::unordered_map<size_t, std::shared_ptr<RootSignature>> g_cache;
        static std::mutex g_mutex;
    };
}
