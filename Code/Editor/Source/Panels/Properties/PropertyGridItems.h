#pragma once

//#include <nana/gui/widgets/pgitems.hpp>

#include <Berta/Controls/Button.h>
#include <Berta/Controls/InputText.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	/*class pg_vector3
		: public Berta::pgitem
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
		virtual void create(Berta::Window wd) override;

		virtual void draw(Berta::paint::graphics* graph, Berta::rectangle area, unsigned labelw, unsigned  valuew, unsigned  iboxw, const int txtoff, Berta::color bgcolor, Berta::color fgcolor) const override;
		
		mutable Berta::textbox	xyz_[3];
		Math::Vector3	vector3_;
	};

	class pg_asset_file
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