#pragma once

#include <optional>

#include "Berta/Controls/Properties/PropertyGridFieldBase.h"
#include <Berta/Controls/Button.h>
#include <Berta/Controls/TextBox.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class PropertyGridFieldVector3 : public Berta::Internal::PropertyGrid::PropertyGridFieldBase
	{
	public:
		using GetterFn = std::function<std::optional<Math::Vector3>()>;
		using SetterFn = std::function<void(const Math::Vector3&)>;
		
	public:
		PropertyGridFieldVector3(std::string_view label, GetterFn getter, SetterFn setter) :
			PropertyGridFieldBase(label), m_getter(std::move(getter)), m_setter(std::move(setter))
		{
		}
		virtual void Draw(Berta::Graphics& graphics, const Berta::Rectangle& area, const LayoutConfig& config) override;
		
		virtual void SetFocus() override;
		virtual void Refresh() override;
		std::string GetValueAsString() const override;
		/*pg_vector3() = default;

		pg_vector3(const std::string& label, const std::string& value)
			: pgitem(label, value)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void value(Math::Vector3 value);
		virtual Math::Vector3 to_vector3() const
		{
			return vector3_;
		}*/

	protected:
		virtual void OnCreate(Berta::Window* parent) override;
		virtual void OnVisibilityChanged(bool visible) override;
		virtual void OnEnableChanged(bool enabled) override;
		/*virtual void create(Berta::Window wd) override;

		virtual void draw(Berta::paint::graphics* graph, Berta::rectangle area, unsigned labelw, unsigned  valuew, unsigned  iboxw, const int txtoff, Berta::color bgcolor, Berta::color fgcolor) const override;
		
		mutable Berta::textbox	xyz_[3];
		Math::Vector3	vector3_;*/
		
		GetterFn m_getter;
		SetterFn m_setter;
	};

	/*class pg_asset_file
		: public Berta::pg_string_button
	{
	public:
		pg_asset_file() = default;

		pg_asset_file(const std::string& label, const std::string& value)
			: pg_string_button(label, value)
		{}

		virtual void value(const std::string& value) override;

	protected:
		virtual void create(Berta::Window wd) override;
	};*/
}
