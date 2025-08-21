#include "brepch.h"
#include "PropertyGridItems.h"

namespace Bruno
{
	//void pg_vector3::value(const std::string& value)
	//{
	//	std::stringstream ss(value);
	//	std::string item;
	//	std::vector<float> items;

	//	try
	//	{
	//		while (getline(ss, item, '/'))
	//			items.push_back(item.empty() ? 0 : std::stof(item));
	//	}
	//	catch (...)
	//	{
	//		// reset
	//		items.clear();
	//	}

	//	//Berta::internal_lock_guard evt_lock(&evt_emit_, false);

	//	for (size_t i = 0; i < 3; ++i)
	//		xyz_[i].caption(std::to_string(items[i]));

	//	vector3_ = Math::Vector3(items[0], items[1], items[2]);
	//	pgitem::value(std::to_string(items[0]) + "/" + std::to_string(items[1]) + "/" + std::to_string(items[2]));
	//}

	//void pg_vector3::enabled(bool state)
	//{
	//	pgitem::enabled(state);
	//	for (auto& i : xyz_)
	//	{
	//		i.enabled(en_);
	//	}
	//}

	//void pg_vector3::value(Math::Vector3 value)
	//{
	//	pgitem::value(std::to_string(value.x) + "/" + std::to_string(value.y) + "/" + std::to_string(value.z));
	//}

	//void pg_vector3::create(Berta::Window wd)
	//{
	//	// textboxes
	//	for (auto& i : xyz_)
	//	{
	//		i.create(wd);
	//		i.multi_lines(false);
	//		i.focus_behavior(Berta::textbox::text_focus_behavior::select_if_tabstop_or_click);

	//		i.events().click.connect_front([this](const Berta::arg_click& arg)
	//		{
	//			scroll();
	//		});
	//		i.events().dbl_click([this, &i](const Berta::arg_mouse& arg)
	//		{
	//			i.select(true);
	//		});
	//		i.events().key_press([this](const Berta::arg_keyboard& arg)
	//		{
	//			if (arg.key == Berta::keyboard::enter)
	//			{
	//				std::string new_value = xyz_[0].caption() + "/" + xyz_[1].caption() + "/" + xyz_[2].caption();
	//				if (new_value != pgitem::value())
	//				{
	//					pg_vector3::value(new_value);
	//					emit_event();
	//				}
	//			}
	//		});
	//		i.events().focus([this, &i](const Berta::arg_focus& arg)
	//		{
	//			// on lost focus: capture the value left by the user
	//			if (!arg.getting)
	//			{
	//				std::string new_value = xyz_[0].caption() + "/" + xyz_[1].caption() + "/" + xyz_[2].caption();
	//				if (new_value != pgitem::value())
	//				{
	//					pg_vector3::value(new_value);
	//					emit_event();
	//				}
	//			}
	//		});
	//		i.set_accept([&i](wchar_t c) -> bool
	//		{
	//			return (isdigit(c) || (c == '.' && i.text().find('.') == std::string::npos) ||
	//				c == Berta::keyboard::cancel || c == Berta::keyboard::backspace || c == Berta::keyboard::tab) ? true : false;
	//		});
	//	}

	//	pg_vector3::value(value_);
	//}

	//void pg_vector3::draw(Berta::paint::graphics* graph, Berta::rectangle area, unsigned labelw, unsigned valuew, unsigned iboxw, const int txtoff, Berta::color bgcolor, Berta::color fgcolor) const
	//{
	//	// background
	//	graph->rectangle(area, true, bgcolor);

	//	// draw label
	//	if (labelw)
	//	{
	//		auto rect = area;
	//		rect.width = labelw;
	//		draw_label(graph, rect, txtoff, bgcolor, fgcolor);
	//	}

	//	for (auto& i : xyz_)
	//		Berta::API::show_window(i, true);

	//	if (valuew)
	//	{
	//		auto availw = valuew;
	//		auto txtsize = graph->text_extent_size("X");
	//		const unsigned txtw = txtsize.width + 5;
	//		const int offset = 8;
	//		int y = size_ + static_cast<int>(size_ - txtsize.height) / 2;
	//		int ctrly = 0;
	//		const unsigned ctrlw = (availw - (3 * txtw) - (2 * offset)) / 3;
	//		auto rect = area;
	//		rect.x += labelw;

	//		// X
	//		graph->string(Berta::point{ rect.x, rect.y }, "X", fgcolor);
	//		rect.x += txtw;

	//		xyz_[0].move(rect.x, ctrly);
	//		xyz_[0].size(Berta::size(ctrlw, size_ - 2 * PG_BORDER_Y));
	//		rect.x += ctrlw + offset;

	//		// Y
	//		graph->string(Berta::point{ rect.x, rect.y }, "Y", fgcolor);
	//		rect.x += txtw;

	//		xyz_[1].move(rect.x, ctrly);
	//		xyz_[1].size(Berta::size(ctrlw, size_ - 2 * PG_BORDER_Y));
	//		rect.x += ctrlw + offset;
	//		// Z
	//		graph->string(Berta::point{ rect.x, rect.y }, "Z", fgcolor);
	//		rect.x += txtw;

	//		xyz_[2].move(rect.x, ctrly);
	//		xyz_[2].size(Berta::size(ctrlw, size_ - 2 * PG_BORDER_Y));
	//	}
	//}

	//void pg_asset_file::value(const std::string& value)
	//{
	//	pgitem::value(value);
	//}

	//void pg_asset_file::create(Berta::Window wd)
	//{
	//	pg_string_button::create(wd);

	//	txt_.events().focus.connect_front([this](const Berta::arg_focus& arg)
	//	{
	//		arg.stop_propagation();
	//	});

	//	value(value_);

	//	set_button_click([this](const Berta::arg_click& arg)
	//	{
	//	});
	//}
}