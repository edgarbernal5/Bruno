#include "brpch.h"
#include "GameWindow.h"

namespace Bruno
{
    BR_RTTI_DEFINITIONS(GameWindow);

    uint32_t GameWindow::GetWidth() const
    {
        return m_data.Width;
    }

    uint32_t GameWindow::GetHeight() const
    {
        return m_data.Height;
    }
}