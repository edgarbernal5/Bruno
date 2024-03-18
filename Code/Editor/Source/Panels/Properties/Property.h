#pragma once

#include <string>
#include <vector>
#include <utility>
#include <Bruno/Math/Math.h>
#include <Bruno/Core/Events/Event.h>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	enum class pg_type
	{
		string,
		vector3
	};

	struct property_t
	{
		std::string			name;
		std::string			value;

		std::string			label;
		std::string			category;
		pg_type				type;

		Event<std::string>	on_change;
	};
	
	class property_proxy
	{
	public:
		property_proxy() = default;
		explicit property_proxy(property_t* prop)
			: _prop(prop)
		{}

		property_proxy& category(const std::string& category);
		std::string category() const;

		property_proxy& label(const std::string& label);
		std::string label() const;

		property_proxy& name(const std::string& name);
		std::string name() const;

		property_proxy& value(const std::string& value);
		std::string value() const;

		property_proxy& type(const pg_type type);
		pg_type type() const;

		property_proxy& value(int value);
		property_proxy& value(float value);
		property_proxy& value(Math::Vector3 value);
		property_proxy& value(UUID value);

		Event<std::string>& on_change() { return _prop->on_change; }
		const Event<std::string>& on_change() const { return _prop->on_change; }

		Math::Vector3 as_vector3(Math::Vector3 def = Math::Vector3::Zero) const;

		bool operator== (const property_proxy& other) const
		{
			return _prop == other._prop;
		}
	protected:
		
	private:
		property_t* _prop{ nullptr };
	};

	class properties_collection
	{
	public:
		properties_collection() = default;
		~properties_collection();

		property_proxy append(const std::string& name);
		property_proxy append(const property_t& prop);

		void clear();

		property_proxy get(const std::string& name);

		property_proxy operator[](size_t index);

		size_t size() const;
	protected:
		std::vector<property_t>		m_properties;
	};
}

namespace std
{
	template <>
	struct hash<Bruno::property_proxy>
	{
		std::size_t operator()(const Bruno::property_proxy& key) const
		{
			return std::hash<std::string>{}(key.category()) ^
				(std::hash<std::string>{}(key.name()));
			
		}
	};
}