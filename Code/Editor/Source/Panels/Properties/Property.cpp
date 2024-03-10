#include "Property.h"
#include <sstream>
#include <iomanip>

namespace Bruno
{
    properties_collection::~properties_collection()
    {
        _props.clear();
    }

    property_proxy properties_collection::append(const std::string& name)
    {
        property_t prop;
        prop.name = name;
        _props.push_back(prop);
        return property_proxy{ &_props.back() };
    }

    property_proxy properties_collection::append(const property_t& prop)
    {
        _props.push_back(prop);
        return property_proxy{ &_props.back() };
    }

    void properties_collection::clear()
    {
        _props.clear();
    }
    
    property_proxy properties_collection::get(const std::string& name)
    {
        for (auto i = _props.begin(); i < _props.end(); ++i)
            if (i->name == name)
                return property_proxy{ &*i };

        return property_proxy{};
    }
    
    property_proxy properties_collection::operator[](size_t index)
    {
        return property_proxy{ &_props[index] };
    }

    property_proxy& property_proxy::name(const std::string& name)
    {
        _prop->name = name;
        return *this;
    }

    std::string property_proxy::name() const
    {
        return _prop->name;
    }

    property_proxy& property_proxy::value(const std::string& value)
    {
        _prop->value = value;
        return *this;
    }

    std::string property_proxy::value() const
    {
        return _prop->value;
    }

    property_proxy& property_proxy::value(int value)
    {
        if (_prop)
        {
            _prop->value = std::to_string(value);
        }

        return *this;
    }

    property_proxy& property_proxy::value(float value)
    {
        if (_prop)
        {
            _prop->value = std::to_string(value);
        }

        return *this;
    }

    property_proxy& property_proxy::value(Math::Vector3 value)
    {
        if (_prop)
        {
            std::ostringstream builder;
            builder << std::to_string(value.x) << "/" << std::to_string(value.y) << "/" << std::to_string(value.z);

            _prop->value = builder.str();
        }

        return *this;
    }
}