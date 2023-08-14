#include "brpch.h"
#include "ContentReader.h"

#include "ContentManager.h"
#include "ContentTypeReaderManager.h"

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
        auto object = ReadObject();
        ReadSharedResources(sharedResourceCount);

        return object;
    }

    void ContentReader::ReadChar(char& output)
    {
        m_stream.ReadRaw<char>(output);
    }

    std::shared_ptr<RTTI> ContentReader::ReadExternalReference()
    {
        std::wstring referenceName;
        ReadWString(referenceName);

        if (referenceName.empty())
            return nullptr;

        std::filesystem::path path(referenceName);

        std::filesystem::path cleanPath = path / referenceName;

        return m_contentManager->Load<RTTI>(cleanPath);
    }

    void ContentReader::ReadInt32(int32_t& output)
    {
        m_stream.ReadRaw<int32_t>(output);
    }

    void ContentReader::ReadInt64(int64_t& output)
    {
        m_stream.ReadRaw<int64_t>(output);
    }

    void ContentReader::ReadUInt8(uint8_t& output)
    {
        m_stream.ReadRaw<uint8_t>(output);
    }

    void ContentReader::ReadUInt32(uint32_t& output)
    {
        m_stream.ReadRaw<uint32_t>(output);
    }

    void ContentReader::ReadUInt64(uint64_t& output)
    {
        m_stream.ReadRaw<uint64_t>(output);
    }

    void ContentReader::ReadString(std::string& output)
    {
        m_stream.ReadString(output);
    }

    void ContentReader::ReadWString(std::wstring& output)
    {
        m_stream.ReadWString(output);
    }

    void ContentReader::ReadBytes(std::vector<uint8_t>& output)
    {
        m_stream.ReadBytes(output);
    }

    void ContentReader::ReadSharedResources(uint32_t sharedResourceCount)
    {
        if (sharedResourceCount > 0)
        {
            std::vector<std::shared_ptr<RTTI>> objects(sharedResourceCount, nullptr);
            for (uint32_t i = 0; i < sharedResourceCount; i++)
            {
                objects[i] = ReadObject();
            }

            for (uint32_t i = 0; i < sharedResourceCount; i++)
            {
                for (uint32_t j = 0; j < m_sharedResourceFixups[i].size(); j++)
                {
                    m_sharedResourceFixups[i][j](objects[i]);
                }
            }
        }
    }

    void ContentReader::ReadVector2(Math::Vector2& output)
    {
        m_stream.ReadRaw<Math::Vector2>(output);
    }

    void ContentReader::ReadVector3(Math::Vector3& output)
    {
        m_stream.ReadRaw<Math::Vector3>(output);
    }

    void ContentReader::ReadVector4(Math::Vector4& output)
    {
        m_stream.ReadRaw<Math::Vector4>(output);
    }

    void ContentReader::ReadSharedResource(std::function<void(std::shared_ptr<RTTI>)> action)
    {
        int index;
        ReadInt32(index);

        if (index != 0)
        {
            index--;
            if (index >= m_sharedResourceFixups.size())
            {
                throw std::exception("bad bruno");
            }
            m_sharedResourceFixups[index].push_back(action);
        }
    }

    uint32_t ContentReader::ReadHeader()
    {
        uint64_t readersCount;
        ReadUInt64(readersCount);

        for (size_t i = 0; i < readersCount; i++)
        {
            std::string readerName;
            ReadString(readerName);

            auto reader = ContentTypeReaderManager::GetReaderByName(readerName);
            m_readers.push_back(reader.get());
        }

        uint32_t sharedResources;
        ReadUInt32(sharedResources);

        if (sharedResources > 0)
        {
            m_sharedResourceFixups.resize(sharedResources);
        }

        return sharedResources;
    }

    std::shared_ptr<RTTI> ContentReader::ReadObject()
    {
        int readerIndex;
        ReadInt32(readerIndex);

        if (readerIndex == 0)
            return nullptr;

        --readerIndex;
        if (readerIndex >= m_readers.size())
        {
            throw std::exception("Invalid type reader index.");
        }

        return m_readers[readerIndex]->Read(*this);
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