#pragma once

#include "Berta/Controls/Panel.h"
#include "Berta/Controls/PropertyGrid.h"
#include "Berta/GUI/Layout.h"

namespace Bruno
{
    struct ShadowProperties;

    class ShadowsPanel: public Berta::Panel
    {
    public:
        ShadowsPanel(Berta::Window* window, ShadowProperties* shadowProperties);
        
    private:
		Berta::Layout m_layout;
        Berta::PropertyGrid m_propertyGrid;
        ShadowProperties* m_shadowProperties;
    };
}
