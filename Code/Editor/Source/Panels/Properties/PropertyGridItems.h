#pragma once

#include <nana/gui/widgets/pgitems.hpp>

#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class pg_vector3
		: public nana::pgitem
	{
	public:
		pg_vector3() = default;

		pg_vector3(const std::string& label, const std::string& value)
			: pgitem(label, value)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void value(Math::Vector3 value);
		virtual Math::Vector3 to_vector3() const
		{
			return vector3_;
		}

	protected:
		virtual void create(nana::window wd) override;

		virtual void draw(nana::paint::graphics* graph, nana::rectangle area, unsigned labelw, unsigned  valuew, unsigned  iboxw, const int txtoff, nana::color bgcolor, nana::color fgcolor) const override;
		
		mutable nana::textbox	xyz_[3];
		Math::Vector3	vector3_;
	};

	class pg_asset_file
		: public nana::pg_string_button
	{
	public:
		pg_asset_file() = default;

		pg_asset_file(const std::string& label, const std::string& value)
			: pg_string_button(label, value)
		{}

		virtual void value(const std::string& value) override;

	protected:
		virtual void create(nana::window wd) override;

	};
}