#include "brepch.h"
#include "Property.h"

#include <sstream>
#include <iomanip>
#include <Bruno/Scene/Scene.h>

namespace Bruno
{
    PropertyCollection::~PropertyCollection()
    {
        m_properties.clear();
    }

    PropertyItem PropertyCollection::append(const std::string& name)
    {
        PropertyType prop;
        prop.name = name;
        m_properties.push_back(prop);
        return PropertyItem{ &m_properties.back() };
    }

    PropertyItem PropertyCollection::append(const PropertyType& prop)
    {
        m_properties.push_back(prop);
        return PropertyItem{ &m_properties.back() };
    }

    void PropertyCollection::clear()
    {
        m_properties.clear();
    }
    
    PropertyItem PropertyCollection::get(const std::string& name)
    {
        for (auto i = m_properties.begin(); i < m_properties.end(); ++i)
            if (i->name == name)
            {
                return PropertyItem{ &*i };
            }

        return PropertyItem{};
    }
    
    PropertyItem PropertyCollection::operator[](size_t index)
    {
        return PropertyItem{ &m_properties[index] };
    }

    size_t PropertyCollection::size() const
    {
        return m_properties.size();
    }

    PropertyItem& PropertyItem::SetName(const std::string& name)
    {
        m_prop->name = name;
        return *this;
    }

    std::string PropertyItem::GetName() const
    {
        return m_prop->name;
    }

    std::string PropertyItem::GetCategory() const
    {
        return m_prop->category;
    }

    PropertyItem& PropertyItem::SetCategory(const std::string& category)
    {
        m_prop->category = category;
        return *this;
    }

    PropertyItem& PropertyItem::SetLabel(const std::string& label)
    {
        m_prop->label = label;
        return *this;
    }

    std::string PropertyItem::GetLabel() const
    {
        return m_prop->label;
    }

    PropertyItem& PropertyItem::SetValue(const std::string& value)
    {
        bool emit_event = m_prop->value != value;
        m_prop->value = value;
        if (emit_event)
        {
            m_prop->on_change.emit(value);
        }
        return *this;
    }

    std::string PropertyItem::GetValue() const
    {
        return m_prop->value;
    }

    PropertyItem& PropertyItem::SetType(PropertyGridType type)
    {
        m_prop->type = type;
        return *this;
    }

    PropertyGridType PropertyItem::GetType() const
    {
        return m_prop->type;
    }

    PropertyItem& PropertyItem::SetReadOnly(bool read_only)
    {
        m_prop->is_read_only = read_only;
        return *this;
    }

    bool PropertyItem::IsReadOnly() const
    {
        return m_prop->is_read_only;
    }

    PropertyItem& PropertyItem::SetValue(int value)
    {
        if (m_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = m_prop->value != new_value;
            m_prop->value = new_value;
            if (emit_event)
                m_prop->on_change.emit(new_value);
        }

        return *this;
    }

    PropertyItem& PropertyItem::SetValue(float value)
    {
        if (m_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = m_prop->value != new_value;
            m_prop->value = new_value;
            if (emit_event)
                m_prop->on_change.emit(new_value);
        }

        return *this;
    }

    PropertyItem& PropertyItem::SetValue(uint32_t value)
    {
        if (m_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = m_prop->value != new_value;
            m_prop->value = new_value;
            if (emit_event)
                m_prop->on_change.emit(new_value);
        }

        return *this;
    }

    PropertyItem& PropertyItem::SetValue(Math::Vector3 value)
    {
        if (m_prop)
        {
            std::ostringstream builder;
            builder << std::to_string(value.x) << "/" << std::to_string(value.y) << "/" << std::to_string(value.z);

            auto new_value = builder.str();
            bool emit_event = m_prop->value != new_value;
            m_prop->value = new_value;
            if (emit_event)
                m_prop->on_change.emit(new_value);
        }
        return *this;
    }

    PropertyItem& PropertyItem::SetValue(UUID value)
    {
        if (m_prop)
        {
            auto new_value = std::to_string(value);
            bool emit_event = m_prop->value != new_value;
            m_prop->value = new_value;
            if (emit_event)
                m_prop->on_change.emit(new_value);
        }
        return *this;
    }

    Math::Vector3 PropertyItem::as_vector3(Math::Vector3 def) const
    {
        if (!m_prop || m_prop->value.empty())
            return def;

        std::istringstream builder(m_prop->value);
        Math::Vector3 result;
        char separator;
        builder >> result.x >> separator >> result.y >> separator >> result.z;

        return result;
    }

}