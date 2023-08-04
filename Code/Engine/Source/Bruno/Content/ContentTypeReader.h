#pragma once

#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	class Game;
	class ContentReader;

	class AbstractContentTypeReader : public RTTI
	{
		BR_RTTI_DECLARATION(AbstractContentTypeReader, RTTI);

	public:
		virtual ~AbstractContentTypeReader() = default;

		RTTI::IdType GetTargetTypeId() const;
		virtual std::shared_ptr<RTTI> Read(ContentReader& input) = 0;
	protected:
		AbstractContentTypeReader(const RTTI::IdType targetTypeId);

		const RTTI::IdType m_targetTypeId;
	};

	template <typename T>
	class ContentTypeReader : public AbstractContentTypeReader
	{
	public:
		virtual ~ContentTypeReader() = default;

		virtual std::shared_ptr<RTTI> Read(ContentReader& input) override;

	protected:
		ContentTypeReader(const RTTI::IdType targetTypeId);

		virtual std::shared_ptr<T> ReadInternal(ContentReader& input) = 0;
	};

	template<typename T>
	inline ContentTypeReader<T>::ContentTypeReader(const RTTI::IdType targetTypeId) :
		AbstractContentTypeReader(targetTypeId)
	{
	}

	template<typename T>
	inline std::shared_ptr<RTTI> ContentTypeReader<T>::Read(ContentReader& input)
	{
		return ReadInternal(input);
	}
}