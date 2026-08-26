#include "brepch.h"
#include "ProfilerPanel.h"

#include "Berta/Core/Foundation.h"
#include "Bruno/Platform/DirectX/Profiler.h"

namespace Bruno
{
    ProfilerPanel::ProfilerPanel(Berta::Window* window) :
        Berta::Panel(window)
    {
        m_layout.Create(*this);
        m_visibleEntitiesLabel.Create(*this, false, {0,0,100,100});
        m_totalEntitiesLabel.Create(*this, false, {0,0,100,100});
        m_cpuCullingTimeMsLabel.Create(*this, false, {0,0,100,100});
        m_cpuTotalRenderTimeMsLabel.Create(*this, false, {0,0,100,100});
        m_gpuRenderTimeMsLabel.Create(*this, false, {0,0,100,100});
        
        m_layout.Parse("{VerticalLayout {a}{b}{c}{d}{e}}");

        m_layout.Attach("a", m_totalEntitiesLabel);
        m_layout.Attach("b", m_visibleEntitiesLabel);
        m_layout.Attach("c", m_cpuCullingTimeMsLabel);
        m_layout.Attach("d", m_cpuTotalRenderTimeMsLabel);
        m_layout.Attach("e", m_gpuRenderTimeMsLabel);

        m_visibleEntitiesLabel.SetCaption("Visible Entities:");
        m_totalEntitiesLabel.SetCaption("Total Entities:");
        
        Berta::Foundation::GetInstance().SetOnIdleTickCallback([this]()
        {
            auto& profiler = Profiler::Get();
        
            m_visibleEntitiesLabel.SetCaption(L"Visible Entities: " + std::to_wstring(profiler.Stats.RenderedEntities));
            m_totalEntitiesLabel.SetCaption(L"Total Entities: " + std::to_wstring(profiler.Stats.TotalEntities));
            
            m_cpuCullingTimeMsLabel.SetCaption(L"CPU Culling Time: " + std::to_wstring(profiler.Stats.CpuCullingTimeMs));
            m_cpuTotalRenderTimeMsLabel.SetCaption(L"CPU Render Time: " + std::to_wstring(profiler.Stats.CpuTotalRenderTimeMs));
            m_gpuRenderTimeMsLabel.SetCaption(L"GPU Render Time: " + std::to_wstring(profiler.Stats.GpuRenderTimeMs));
        });
    }
}
