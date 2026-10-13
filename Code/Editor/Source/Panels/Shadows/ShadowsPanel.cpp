#include "brepch.h"
#include "ShadowsPanel.h"

#include "Berta/Controls/Properties/PropertyGridFieldNumeric.h"
#include "Berta/Controls/Properties/PropertyGridFieldSlider.h"
#include "Bruno/Renderer/Shadows/EnumTypes.h"

namespace Bruno
{
    ShadowsPanel::ShadowsPanel(Berta::Window* window, ShadowProperties* shadowProperties):
        Berta::Panel(window),
        m_shadowProperties(shadowProperties)
    {
		this->SetCaption("Properties");
        
		m_propertyGrid.Create(*this);
        
        m_layout.Create(this->Handle());
        ////////// VIEW
        m_layout.Parse("{VerticalLayout {properties}}");

        m_layout.Attach("properties", m_propertyGrid);
        m_layout.Apply();
        
        auto configuration = m_propertyGrid.Append("Configuration");
        configuration.EmplaceProperty<Berta::PropertyGridFieldSliderUInt>("Cascade Count",
            [this]()
            {
                return m_shadowProperties->m_numCascades;
            },
            [this](const uint32_t& val) mutable
            {
                m_shadowProperties->m_numCascades = val;
            }, 1, 4);
        
        configuration.EmplaceProperty<Berta::PropertyGridFieldSliderFloat>("Cascade Split Distribution",
            [this]()
            {
                return m_shadowProperties->m_cascadeLambda;
            },
            [this](const float& val) mutable
            {
                m_shadowProperties->m_cascadeLambda = val;
            }, 0.0f, 1.0f);
    }
}
