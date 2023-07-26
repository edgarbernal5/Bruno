#pragma once

#include <cstdint>
#include <string>
#include <iostream>

namespace Bruno
{
#define BR_RTTI_DECLARATION(Type, ParentType)																		\
	public:																											\
		static IdType GetTypeIdClass() { return g_runTimeTypeId; }													\
		static std::string GetTypeName() { return #Type; }															\
		virtual IdType GetTypeIdInstance() const override { return Type::GetTypeIdClass(); }						\
		virtual RTTI* QueryInterface(const IdType id) override														\
        {																											\
			return (id == g_runTimeTypeId ? reinterpret_cast<RTTI*>(this) : ParentType::QueryInterface(id));		\
        }																											\
		virtual bool Is(IdType id) const override																	\
		{																											\
			return (id == g_runTimeTypeId ? true : ParentType::Is(id));												\
		}																											\
		virtual bool Is(const std::string& name) const override														\
		{																											\
			return (name == GetTypeName() ? true : ParentType::Is(name));											\
		}																											\
	private:																										\
		static RTTI::IdType g_runTimeTypeId;

#define BR_RTTI_DEFINITIONS(Type) RTTI::IdType Type::g_runTimeTypeId = reinterpret_cast<RTTI::IdType>(&Type::g_runTimeTypeId);

	class RTTI
	{
	public:
		using IdType = uint64_t;

		virtual ~RTTI() = default;

		virtual std::uint64_t GetTypeIdInstance() const = 0;

		virtual RTTI* QueryInterface(const IdType) { return nullptr; }

		virtual bool Is(IdType) const { return false; }

		virtual bool Is(const std::string&) const { return false; }

		template <typename T>
		T* As() const
		{
			return (Is(T::GetTypeIdClass()) ? reinterpret_cast<T*>(const_cast<RTTI*>(this)) : nullptr);
		}
	};
}