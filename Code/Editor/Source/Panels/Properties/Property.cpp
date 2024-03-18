#include "Property.h"

#include <sstream>
#include <iomanip>
#include <Bruno/Scene/Scene.h>

namespace Bruno
{
    properties_collection::~properties_collection()
    {
        m_properties.clear();
    }

    property_proxy properties_collection::append(const std::string& name)
    {
        property_t prop;
        prop.name = name;
        m_properties.push_back(prop);
        return property_proxy{ &m_properties.back() };
    }

    property_proxy properties_collection::append(const property_t& prop)
    {
        m_properties.push_back(prop);
        return property_proxy{ &m_properties.back() };
    }

    void properties_collection::clear()
    {
        m_properties.clear();
    }
    
    property_proxy properties_collection::get(const std::string& name)
    {
        for (auto i = m_properties.begin(); i < m_properties.end(); ++i)
            if (i->name == name)
                return property_proxy{ &*i };

        return property_proxy{};
    }
    
    property_proxy properties_collection::operator[](size_t index)
    {
        return property_proxy{ &m_properties[index] };
    }

    size_t properties_collection::size() const
    {
        return m_properties.size();
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

    std::string property_proxy::category() const
    {
        return _prop->category;
    }

    property_proxy& property_proxy::category(const std::string& category)
    {
        _prop->category = category;
        return *this;
    }

    property_proxy& property_proxy::label(const std::string& label)
    {
        _prop->label = label;
        return *this;
    }

    std::string property_proxy::label() const
    {
        return _prop->label;
    }

    property_proxy& property_proxy::value(const std::string& value)
    {
        bool emit_event = _prop->value != value;
        _prop->value = value;
        if (emit_event)
            _prop->on_change.emit(value);
        return *this;
    }

    std::string property_proxy::value() const
    {
        return _prop->value;
    }

    property_proxy& property_proxy::type(const pg_type type)
    {
        _prop->type = type;
        return *this;
    }

    pg_type property_proxy::type() const
    {
        return _prop->type;
    }

    property_proxy& property_proxy::value(int value)
    {
        if (_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = _prop->value != new_value;
            _prop->value = new_value;
            if (emit_event)
                _prop->on_change.emit(new_value);
        }

        return *this;
    }

    property_proxy& property_proxy::value(float value)
    {
        if (_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = _prop->value != new_value;
            _prop->value = new_value;
            if (emit_event)
                _prop->on_change.emit(new_value);
        }

        return *this;
    }

    property_proxy& property_proxy::value(Math::Vector3 value)
    {
        if (_prop)
        {
            std::ostringstream builder;
            builder << std::to_string(value.x) << "/" << std::to_string(value.y) << "/" << std::to_string(value.z);

            auto new_value = builder.str();
            bool emit_event = _prop->value != new_value;
            _prop->value = new_value;
            if (emit_event)
                _prop->on_change.emit(new_value);
        }
        return *this;
    }

    property_proxy& property_proxy::value(UUID value)
    {
        if (_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = _prop->value != new_value;
            _prop->value = new_value;
            if (emit_event)
                _prop->on_change.emit(new_value);
        }
        return *this;
    }

    Math::Vector3 property_proxy::as_vector3(Math::Vector3 def) const
    {
        if (!_prop || _prop->value.empty())
            return def;

        std::istringstream builder(_prop->value);
        Math::Vector3 result;
        char separator;
        builder >> result.x >> separator >> result.y >> separator >> result.z;

        return result;
    }

}