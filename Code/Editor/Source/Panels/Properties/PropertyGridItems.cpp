#include "PropertyGridItems.h"

namespace Bruno
{
	void pg_vector3::value(const std::string& value)
	{
		std::stringstream ss(value);
		std::string item;
		std::vector<float> items;

		try
		{
			while (getline(ss, item, '/'))
				items.push_back(item.empty() ? 0 : std::stof(item));
		}
		catch (...)
		{
			// reset
			items.clear();
		}

		//nana::internal_lock_guard evt_lock(&evt_emit_, false);

		for (size_t i = 0; i < 3; ++i)
			xyz_[i].caption(std::to_string(items[i]));
	}

	void pg_vector3::enabled(bool state)
	{
		pgitem::enabled(state);
		for (auto& i : xyz_)
		{
			i.enabled(en_);
		}
	}

	void pg_vector3::value(Math::Vector3 value)
	{
	}

	void pg_vector3::create(nana::window wd)
	{
		// textboxes
		for (auto& i : xyz_)
		{
			i.create(wd);
			i.multi_lines(false);
			i.focus_behavior(nana::textbox::text_focus_behavior::select_if_tabstop_or_click);

			i.events().click.connect_front([this](const nana::arg_click& arg)
			{
				scroll();
			});
			i.events().dbl_click([this, &i](const nana::arg_mouse& arg)
			{
				i.select(true);
			});
			i.events().key_press([this](const nana::arg_keyboard& arg)
			{
				if (arg.key == nana::keyboard::enter)
				{
					std::string col = xyz_[0].caption() + "/" + xyz_[1].caption() + "/" + xyz_[2].caption();
					if (col != pgitem::value())
					{
						pg_vector3::value(xyz_[0].caption() + "/" + xyz_[1].caption() + "/" + xyz_[2].caption());
						emit_event();
					}
				}
			});
			i.events().focus([this, &i](const nana::arg_focus& arg)
			{
				// on lost focus: capture the value left by the user
				if (!arg.getting)
				{
					
				}
			});
			i.set_accept([](wchar_t c) -> bool
			{
				return (isdigit(c) || c == nana::keyboard::cancel || c == nana::keyboard::backspace || c == nana::keyboard::tab) ? true : false;
			});
		}

		pg_vector3::value(value_);
	}

	void pg_vector3::draw(nana::paint::graphics* graph, nana::rectangle area, unsigned labelw, unsigned valuew, unsigned iboxw, const int txtoff, nana::color bgcolor, nana::color fgcolor) const
	{
		// background
		graph->rectangle(area, true, bgcolor);

		// draw label
		if (labelw)
		{
			auto rect = area;
			rect.width = labelw;
			draw_label(graph, rect, txtoff, bgcolor, fgcolor);
		}

		for (auto& i : xyz_)
			nana::API::show_window(i, true);

		if (valuew)
		{
			auto availw = valuew;
			auto txtsize = graph->text_extent_size("X");
			const unsigned txtw = txtsize.width + 5;
			const int offset = 8;
			int y = size_ + static_cast<int>(size_ - txtsize.height) / 2;
			int ctrly = 0;
			const unsigned ctrlw = (availw - (3 * txtw) - (2 * offset)) / 3;
			auto rect = area;
			rect.x += labelw;

			// X
			graph->string(nana::point{ rect.x, rect.y }, "X", fgcolor);
			rect.x += txtw;

			xyz_[0].move(rect.x, ctrly);
			xyz_[0].size(nana::size(ctrlw, size_ - 2 * PG_BORDER_Y));
			rect.x += ctrlw + offset;

			// Y
			graph->string(nana::point{ rect.x, rect.y }, "Y", fgcolor);
			rect.x += txtw;

			xyz_[1].move(rect.x, ctrly);
			xyz_[1].size(nana::size(ctrlw, size_ - 2 * PG_BORDER_Y));
			rect.x += ctrlw + offset;
			// Z
			graph->string(nana::point{ rect.x, rect.y }, "Z", fgcolor);
			rect.x += txtw;

			xyz_[2].move(rect.x, ctrly);
			xyz_[2].size(nana::size(ctrlw, size_ - 2 * PG_BORDER_Y));
		}
	}
}