#include "brpch.h"
#include "GameWindow.h"

namespace Bruno
{
    uint32_t GameWindow::GetWidth() const
    {
        return m_data.Width;
    }

    uint32_t GameWindow::GetHeight() const
    {
        return m_data.Height;
    }
}