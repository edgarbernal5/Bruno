#include "brpch.h"
#include "Window.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(Window);

    uint32_t Window::GetWidth() const
    {
        return m_data.Width;
    }

    uint32_t Window::GetHeight() const
    {
        return m_data.Height;
    }
}