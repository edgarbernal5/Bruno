#include "brepch.h"
#include "PropertyGridItems.h"

#include "Berta/GUI/EnumTypes.h"

namespace Bruno
{
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

	void PropertyGridFieldVector3::Draw(Berta::Graphics& graphics, const Berta::Rectangle& area, const LayoutConfig& config)
	{
		auto labelWidth = graphics.GetTextExtent("X").Width;
		auto labelWidthWithPadding = labelWidth + 5;
		auto textBoxMargin = labelWidth + 5;
		auto finalTextBoxWidth = area.Width / 3 - textBoxMargin - labelWidthWithPadding;
		
		auto textArea = area;
		textArea.X += (int)labelWidthWithPadding;
		textArea.Width = finalTextBoxWidth;
		for (auto& textBox : m_xyz)
		{
			textBox.SetArea(textArea);
			textArea.X += (int)finalTextBoxWidth + labelWidthWithPadding + textBoxMargin;
		}
	}

	void PropertyGridFieldVector3::SetFocus()
	{
	}

	bool PropertyGridFieldVector3::HasFocus() const
	{
		return false;
	}

	std::wstring PropertyGridFieldVector3::GetValueAsString() const
	{
		return L"";
	}

	void PropertyGridFieldVector3::OnCreate(Berta::Window* parent)
	{
		for (auto& textBox : m_xyz)
		{
			textBox.Create(parent);
			textBox.SetMultiLine(false);
			textBox.SetFocusBehavior(Berta::TextFocusBehavior::SelectOnClick);
			textBox.SetScrollBarVisibility(Berta::ScrollBarVisibility::Hidden);

			Refresh();
			
			textBox.GetEvents().KeyPressed.Connect([this](const Berta::ArgKeyboard& args)
			{
				if (args.Key == Berta::KeyboardKey::Enter)
				{
					//ApplyValue();
				}
			});
			textBox.GetEvents().Focus.Connect([this](const Berta::ArgFocus& args)
			{
				if (args.Focused)
				{
					NotifySelected();
					return;
				}

				//ApplyValue();
			});
		}
	}

	void PropertyGridFieldVector3::OnVisibilityChanged(bool visible)
	{
		for (auto& textBox : m_xyz)
		{
			if (visible)
			{
				textBox.Show();
			}
			else
			{
				textBox.Hide();
			}
		}
	}

	void PropertyGridFieldVector3::OnEnableChanged(bool enabled)
	{
		for (auto& textBox : m_xyz)
		{
			textBox.SetEnabled(enabled);
		}
	}

	void PropertyGridFieldVector3::SetValueInternal(const Math::Vector3& value)
	{
		m_xyz[0].SetCaption(std::to_wstring(value.x));
		m_xyz[1].SetCaption(std::to_wstring(value.y));
		m_xyz[2].SetCaption(std::to_wstring(value.z));
	}

	void PropertyGridFieldVector3::SetMixedValuesInternal()
	{
	}
}
