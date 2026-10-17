#include "brepch.h"
#include "ShadowsPanel.h"

#include "Berta/Controls/Properties/PropertyGridFieldNumeric.h"
#include "Berta/Controls/Properties/PropertyGridFieldSelection.h"
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
        
        configuration.EmplaceProperty<Berta::PropertyGridFieldSelection<uint32_t>>
        (
            "Shadow Map Resolution", 
            [this]() { return m_shadowProperties->m_resolution; },
            [this](const uint32_t& val) { m_shadowProperties->m_resolution = val; },
			
            std::vector<std::pair<std::wstring, uint32_t>>
            {
                { L"1024", 1024 },
                { L"2048", 2048 },
                { L"4048", 4048 }
            }
        );
        
        configuration.EmplaceProperty<Berta::PropertyGridFieldInt>("Depth Bias",
            [this]()
            {
                return m_shadowProperties->m_depthBias;
            },
            [this](const int& val) mutable
            {
                m_shadowProperties->m_depthBias = val;
            });
        
        configuration.EmplaceProperty<Berta::PropertyGridFieldFloat>("Slope-Scaled Depth Bias",
            [this]()
            {
                return m_shadowProperties->m_slopeScaledDepthBias;
            },
            [this](const float& val) mutable
            {
                m_shadowProperties->m_slopeScaledDepthBias = val;
            });
        
        configuration.EmplaceProperty<Berta::PropertyGridFieldSelection<ShadowFilterQuality>>
        (
            "Filter Quality", 
            [this]() { return m_shadowProperties->m_filterQuality; },
            [this](const ShadowFilterQuality& val) { m_shadowProperties->m_filterQuality = val; },
			
            std::vector<std::pair<std::wstring, ShadowFilterQuality>>
            {
                { L"PCF3x3", ShadowFilterQuality::PCF3x3 },
                { L"PCF5x5", ShadowFilterQuality::PCF5x5 },
                { L"PCSS", ShadowFilterQuality::PCSS }
            }
        );
    }
}
