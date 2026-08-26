#pragma once

#include "Berta/Controls/Label.h"
#include "Berta/Controls/Panel.h"
#include "Berta/GUI/Layout.h"

namespace Bruno
{
    class ProfilerPanel : public Berta::Panel
    {
    public:
        ProfilerPanel(Berta::Window* window);
        
    private:
		Berta::Layout m_layout;
        Berta::Label m_visibleEntitiesLabel;
        Berta::Label m_totalEntitiesLabel;
        
        Berta::Label m_cpuCullingTimeMsLabel;
        Berta::Label m_cpuTotalRenderTimeMsLabel;
        Berta::Label m_gpuRenderTimeMsLabel;
    };
}
