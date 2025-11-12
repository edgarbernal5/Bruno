#pragma once

#include <string>
#include <vector>
#include <utility>
#include <Bruno/Math/Math.h>
#include <Bruno/Core/Events/Event.h>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	enum class PropertyGridType
	{
		Uint,
		String,
		Vector3,
		AssetFile,
		Texture
	};

	struct PropertyType
	{
		std::string name;
		std::string value;

		std::string label;
		std::string category;
		PropertyGridType type;
		bool is_read_only{ false };
		Event<std::string> on_change;
	};
	
	class PropertyItem
	{
	public:
		PropertyItem() = default;
		explicit PropertyItem(PropertyType* prop)
			: m_prop(prop)
		{}

		PropertyItem& SetCategory(const std::string& category);
		std::string GetCategory() const;

		PropertyItem& SetLabel(const std::string& label);
		std::string GetLabel() const;

		PropertyItem& SetName(const std::string& name);
		std::string GetName() const;

		PropertyItem& SetValue(const std::string& value);
		std::string GetValue() const;

		PropertyItem& SetType(PropertyGridType type);
		PropertyGridType GetType() const;

		PropertyItem& SetReadOnly(bool read_only);
		bool IsReadOnly() const;

		PropertyItem& SetValue(int value);
		PropertyItem& SetValue(float value);
		PropertyItem& SetValue(uint32_t value);
		PropertyItem& SetValue(Math::Vector3 value);
		PropertyItem& SetValue(UUID value);

		Event<std::string>& on_change() { return m_prop->on_change; }
		const Event<std::string>& on_change() const { return m_prop->on_change; }

		Math::Vector3 as_vector3(Math::Vector3 def = Math::Vector3::Zero) const;

		bool operator== (const PropertyItem& other) const
		{
			return m_prop == other.m_prop;
		}
	protected:
		
	private:
		PropertyType* m_prop{ nullptr };
	};

	class PropertyCollection
	{
	public:
		PropertyCollection() = default;
		~PropertyCollection();

		PropertyItem append(const std::string& name);
		PropertyItem append(const PropertyType& prop);

		void clear();

		PropertyItem get(const std::string& name);

		PropertyItem operator[](size_t index);

		size_t size() const;
	protected:
		std::vector<PropertyType>		m_properties;
	};
}

namespace std
{
	template <>
	struct hash<Bruno::PropertyItem>
	{
		std::size_t operator()(const Bruno::PropertyItem& key) const noexcept
		{
			return std::hash<std::string>{}(key.GetCategory()) ^
				(std::hash<std::string>{}(key.GetName()));
			
		}
	};
}