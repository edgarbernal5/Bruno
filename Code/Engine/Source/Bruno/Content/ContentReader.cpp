#include "brpch.h"
#include "ContentReader.h"

#include "ContentManager.h"

namespace Bruno
{
    ContentReader::ContentReader(ContentManager* contentManager, Stream& stream, const std::wstring& assetName) :
        m_contentManager(contentManager),
        m_stream(stream)
    {
        auto b = ReadChar();
        auto r = ReadChar();
        auto u = ReadChar();
        auto n = ReadChar();
        auto o = ReadChar();
        if (b != 'B' ||
            r != 'R' ||
            u != 'U' ||
            n != 'N' ||
            o != 'O')
        {
            int aaa = 3;
        }

        int totalSizeInBytes = ReadInt32();
    }

    char ContentReader::ReadChar()
    {
        uint8_t byte;
        if (m_stream.Read(&byte, 1) == -1)
        {
            throw std::exception("Error Read()");
        }
        return (char)byte;
    }

    int ContentReader::ReadInt32()
    {
        uint8_t buffer[4];
        if (m_stream.Read(buffer, 4) == -1)
        {
            throw std::exception("Error Read()");
        }
        return (buffer[0]) |
            (buffer[1]) << 8 |
            (buffer[2]) << 16 |
            (buffer[3]) << 24;
    }
}