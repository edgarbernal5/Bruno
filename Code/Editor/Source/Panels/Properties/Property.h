#pragma once

#include <string>
#include <vector>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	struct property_t
	{
		std::string		name;
		std::string		value;

		std::string		label;
		std::string		category;
	};
	
	class property_proxy
	{
	public:
		property_proxy() = default;
		explicit property_proxy(property_t* prop)
			: _prop(prop)
		{}

		property_proxy& name(const std::string& name);
		std::string name() const;

		property_proxy& value(const std::string& value);
		std::string value() const;

		property_proxy& value(int value);
		property_proxy& value(float value);
		property_proxy& value(Math::Vector3 value);

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

	protected:
		std::vector<property_t>		_props;
	};
}