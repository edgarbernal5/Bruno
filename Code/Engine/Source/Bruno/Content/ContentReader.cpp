#include "brpch.h"
#include "ContentReader.h"

#include "ContentManager.h"

namespace Bruno
{
    ContentReader::ContentReader(ContentManager* contentManager, Stream& stream, const std::wstring& assetName) :
        m_contentManager(contentManager),
        m_stream(stream)
    {
        char stamp[5];
        m_stream.Read((uint8_t*)stamp, 5);
        if (stamp[0] != 'B' ||
            stamp[1] != 'R' ||
            stamp[2] != 'U' ||
            stamp[3] != 'N' ||
            stamp[4] != 'O')
        {
            throw std::exception("bad bruno file. header.");
        }

        int totalSizeInBytes;
        ReadInt32(totalSizeInBytes);

        const int HeaderSize = 5; // "BRUNO"
        int headerAndContentSize = HeaderSize + sizeof(int);
        if (totalSizeInBytes - headerAndContentSize > m_stream.GetLength() - m_stream.GetPosition())
        {
            throw std::exception("corrupt file.");
        }
    }

    std::shared_ptr<RTTI> ContentReader::ReadAsset()
    {
        uint32_t sharedResourceCount = ReadHeader();

        return std::shared_ptr<RTTI>();
    }

    void ContentReader::ReadChar(char& output)
    {
        m_stream.ReadRaw<char>(output);
    }

    void ContentReader::ReadInt32(int32_t& output)
    {
        m_stream.ReadRaw<int32_t>(output);
    }

    void ContentReader::ReadUInt32(uint32_t& output)
    {
        m_stream.ReadRaw<uint32_t>(output);
    }

    void ContentReader::ReadString(std::string& output)
    {
        m_stream.ReadString(output);
    }

    uint32_t ContentReader::ReadHeader()
    {
        auto len = m_stream.GetLength();
        auto pos = m_stream.GetPosition();
        uint32_t readersCount;
        ReadUInt32(readersCount);

        for (size_t i = 0; i < readersCount; i++)
        {
            std::string readerName;
            ReadString(readerName);
        }

        return 0;
    }

    /*int ContentReader::ReadInt32()
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
    }*/
}